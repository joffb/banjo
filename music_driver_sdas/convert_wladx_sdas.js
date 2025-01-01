//
// Converts instructions from wla-dx to sdas format
// e.g.		ld a, 4 => ld a, #4
//			ld a, (ix + 4) => ld a, 4(ix)
//
// Joe Kennedy 2024
//

const fs = require('fs');
const readline = require('readline');

const register_names = ["a", "b", "c", "d", "e", "h", "l", "af", "bc", "de", "hl", "ix", "iy", "sp"];

function rearrange(tokens)
{
	if (tokens.length == 0)
	{
		return tokens;
	}
	
	if (tokens.length == 1)
	{
		// just return tokens if it's a register on its own
		if (register_names.indexOf(tokens[0]) != -1)
		{
			return tokens;
		}
	}
	
	// if the first character is an upper/lower byte selector, make the following an immediate value
	if (tokens[0] == "<" || tokens[0] == ">")
	{
		tokens.splice(0, 0, "#");
		return tokens;
	}
	
	// either immediate addressing or using index register
	if (tokens[0] == "(")
	{
		// index registers
		if ((tokens[1] == "ix" || tokens[1] == "iy") && tokens[2] == "+")
		{
			return reorganise_ix_iy(tokens);
		}

		return tokens;
	}
	else
	{
		tokens.splice(0, 0, "#");
		return tokens;
	}
}

// transforms
// (ix + 5) -> 5(ix)
// (ix + n) -> n(ix)
function reorganise_ix_iy(tokens)
{
	let out = tokens.slice(3, tokens.length - 1);
	out.push("(")
	out.push(tokens[1]);
	out.push(")");
	return out;
}

// split asm line into tokens
async function tokenize_lines()
{
	let full_output = [];
	
	let fileStream = fs.createReadStream(process.argv[2]);

	const rl = readline.createInterface({
		input: fileStream,
		crlfDelay: Infinity,
	});

	for await (const line of rl)
	{
		let output = [];
		let accumulator = "";
		
		let inside_instruction = false;

		for (var i = 0; i < line.length; i++)
		{
			// start of comment
			if (line[i] == ";")
			{
				if (accumulator.length > 0)
				{
					output.push(accumulator);
					accumulator = "";
				}
								
				output.push(line[i]);
				output.push(line.substring(i + 1, line.length));
				break;
			}
			
			// whitespace
			else if (" \t".indexOf(line[i]) != -1)
			{
				if (accumulator.length > 0)
				{
					output.push(accumulator);
					accumulator = "";
				}
			
				// if this whitespace came before the instruction
				// then keep it to preserve indentation, otherwise ignore it
				if (inside_instruction == false)
				{
					output.push(line[i]);
				}
			}
			
			// these chars usually delimit in some way
			else if (",()+-|&<>".indexOf(line[i]) != -1)
			{
				inside_instruction = true;
				
				if (accumulator.length > 0)
				{
					output.push(accumulator);
					accumulator = "";
				}
				
				output.push(line[i]);
			}
			
			// append this character to the accumulator
			else
			{
				inside_instruction = true;
				
				accumulator = accumulator + line[i];
			}
			
			// write the accumulator if this is the last character
			if (i == line.length - 1 && accumulator.length > 0)
			{
				output.push(accumulator);
				accumulator = "";
			}
		}
		
		full_output.push(output);
	}
	
	return full_output;
}

// process the tokens
async function process_lines(lines)
{
	// we're expecting certain labels to be declared in C or used in C
	// add an underscore in front of these symbols so they match the C declared ones
	let underscore_prefix = [
		"song_channels", "song_channel_ptrs", "song_state", "song_table", 
		"sfx_table", "sfx_channel", "sfx_state",
	];

	var outfile = fs.createWriteStream(process.argv[3], {flags: "w+"});
	
	//console.log(lines);
	
	for (let i = 0; i < lines.length; i++)
	{
		let line = lines[i];
		
		let opcode = null;
		let dest = [];
		let source = [];		
		
		let whitespace = "";
		let comment = "";
		
		let search = "opcode";
		
		for (let j = 0; j < line.length; j++)
		{
			let token = line[j];
			
			// handle comments
			if (token == ";")
			{
				comment = ";" + line[j + 1];
				j++;
			}
			
			// handle whitespace
			else if (token == " " || token == "\t")
			{
				whitespace += token;
			}
		
			// found the opcode
			else if (search == "opcode")
			{
				if (underscore_prefix.indexOf(token) != -1 || token.indexOf("banjo") === 0)
				{
					token = "_" + token;
				}

				opcode = token;
				search = "dest";
			}
			
			// found a destination token
			else if (search == "dest")
			{
				if (token == ",")
				{
					search = "source";
				}
				else
				{
					dest.push(token);
				}
			}
			
			// found a source token
			else if (search == "source")
			{
				source.push(token);
			}
		}

		// add underscore to front of selected symbols
		for (let k = 0; k < dest.length; k++)
		{
			if (underscore_prefix.indexOf(dest[k]) != -1 || dest[k].indexOf("banjo") === 0)
			{
				dest[k] = "_" + dest[k];
			}
		}

		for (let k = 0; k < source.length; k++)
		{
			if (underscore_prefix.indexOf(source[k]) != -1 || source[k].indexOf("banjo") === 0)
			{
				source[k] = "_" + source[k];
			}
		}

		//
		// update opcode parameters to sdas formatting
		//
		
		// make port for out immediate if it's not register c
		if (opcode == "out")
		{
			if (dest[0] == "(" && dest[1] != "c")
			{
				dest.splice(1, 0, "#");
			}
		}
		
		else if (opcode == "in")
		{
			if (source[0] == "(" && source[1] != "c")
			{
				source.splice(1, 0, "#");
			}
		}
		
		else if (opcode == "ld")
		{
			source = rearrange(source);
			dest = rearrange(dest);
		}
		
		else if (opcode == "cp" || opcode == "add" || opcode == "adc" || opcode == "sub" || opcode == "sbc" || opcode == "and" || opcode == "or" || opcode == "xor")
		{
			source = rearrange(source);
		}
		
		else if (opcode == "inc" || opcode == "dec")
		{
			dest = rearrange(dest);
		}
		
		else if (opcode == "bit" || opcode == "set" || opcode == "res")
		{
			source = rearrange(source);
		}
		
		outfile.write(
			whitespace + 
			(opcode ? (opcode + " ") : "") + 
			(dest.length ? dest.join("") : "") +
			(source.length ? (", " + source.join("")) : "") +
			comment + 
			"\n"
		);
	}
	
	//console.log(output);
}

async function main()
{
	let lines = await tokenize_lines();
	process_lines(lines);
}

main();