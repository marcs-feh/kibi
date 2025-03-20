delimiters = {
	{'(', 'ParenOpen'},
	{')', 'ParenClose'},
	{'[', 'SquareOpen'},
	{']', 'SquareClose'},
	{'{', 'CurlyOpen'},
	{'}', 'CurlyClose'},

	{'=', 'Assign'},
	{'.', 'Dot'},
	{';', 'Semicolon'},
	{',', 'Comma'},

	{'+', 'Plus'},
	{'-', 'Minus'},
	{'*', 'Star'},
	{'/', 'Slash'},
	{'%', 'Modulo'},

	{'==', 'Equal'},
	{'!=', 'NotEqual'},
	{'>', 'Greater'},
	{'<', 'Less'},
	{'>=', 'GreaterEqual'},
	{'<=', 'LesserEqual'},
}

keywords = {
	'let', 'fn', 'return', 'if', 'else', 'for', 'break', 'continue',
}

special = {
	'Unknown', 'String', 'Char', 'Id', 'Integer', 'Real',
}

function Capitalize(s)
	return s:sub(1, 1):upper() .. s:sub(2, #s)
end

--- Enum definition
do
	local lines = {'enum TokenType {'}

	for _, tok in ipairs(special) do
		local def = ('\tTokenType_%s,'):format(tok)
		lines[#lines+1] = def
	end

	for _, tok in ipairs(keywords) do
		local def = ('\tTokenType_%s,'):format(Capitalize(tok))
		lines[#lines+1] = def
	end

	for _, tok in ipairs(delimiters) do
		local def = ('\tTokenType_%s,'):format(tok[2])
		lines[#lines+1] = def
	end

	lines[#lines+1] = '};'

	print(table.concat(lines, '\n'))
end

--- String formatting
do
	local lines = {'static const String token_type_strings[] = {'}

	for _, tok in ipairs(special) do
		local def = ('\t[TokenType_%s] = {},'):format(tok)
		lines[#lines+1] = def
	end

	for _, tok in ipairs(delimiters) do
		local def = ('\t[TokenType_%s] = str_literal("%s"),'):format(tok[2], tok[1])
		lines[#lines+1] = def
	end

	for _, tok in ipairs(keywords) do
		local def = ('\t[TokenType_%s] = str_literal("%s"),'):format(Capitalize(tok), tok)
		lines[#lines+1] = def
	end

	lines[#lines+1] = '};'
	print(table.concat(lines, '\n'))
end
