delimiters = {
	{'(', 'ParenOpen'},
	{')', 'ParenClose'},
	{'[', 'SquareOpen'},
	{']', 'SquareClose'},
	{'{', 'CurlyOpen'},
	{'}', 'CurlyClose'},

	{'=', 'Assign'},
	{'.', 'Dot'},
	{',', 'Comma'},
	{':', 'Colon'},
	{';', 'Semicolon'},

	{'+', 'Plus'},
	{'-', 'Minus'},
	{'*', 'Star'},
	{'/', 'Slash'},
	{'%', 'Modulo'},

	{'+=', 'PlusAssign'},
	{'-=', 'MinusAssign'},
	{'*=', 'StarAssign'},
	{'/=', 'SlashAssign'},
	{'%=', 'ModuloAssign'},

	{'>>', 'ShiftRight'},
	{'<<', 'ShiftLeft'},
	{'&', 'And'},
	{'|', 'Or'},
	{'~', 'Tilde'},

	{'&=', 'AndAssign'},
	{'|=', 'OrAssign'},

	{'->', 'Arrow'},

	{'==', 'Equal'},
	{'!=', 'NotEqual'},
	{'>', 'Greater'},
	{'<', 'Less'},
	{'>=', 'GreaterEqual'},
	{'<=', 'LesserEqual'},
}

keywords = {
	'let', 'fn', 'return', 'if', 'else', 'for', 'break', 'continue',
	'and', 'or', 'not',
}

special = {
	'Unknown', 'String', 'Char', 'Id', 'Integer', 'Real',
}

function Capitalize(s)
	return s:sub(1, 1):upper() .. s:sub(2, #s)
end

Unpack = table.unpack

function Sort(arr, cmp)
	local n = #arr
	for i = 2, n do
		local key = arr[i]
		local j = i - 1
		while j > 0 and cmp(arr[j], key) > 0  do
			arr[j + 1] = arr[j]
			j = j - 1
		end
		arr[j + 1] = key
	end
	return arr
end

function Append(tbl, ...)
	assert(type(tbl) == 'table', 'Expected table got:'.. type(tbl))
	local items = {...}
	for _, v in ipairs(items) do
		tbl[#tbl+1] = v
	end
end

--- Enum definition
do
	local lines = {'enum TokenType {'}

	for _, tok in ipairs(special) do
		local def = ('\tTokenType_%s,'):format(tok)
		Append(lines, def)
	end

	for _, tok in ipairs(keywords) do
		local def = ('\tTokenType_%s,'):format(Capitalize(tok))
		Append(lines, def)
	end

	for _, tok in ipairs(delimiters) do
		local def = ('\tTokenType_%s,'):format(tok[2])
		Append(lines, def)
	end

	Append(lines, '};')

	print(table.concat(lines, '\n'))
end

--- String formatting
do
	local lines = {'static const String token_type_strings[] = {'}

	for _, tok in ipairs(special) do
		local def = ('\t[TokenType_%s] = {},'):format(tok)
		Append(lines, def)
	end

	for _, tok in ipairs(delimiters) do
		local def = ('\t[TokenType_%s] = str_literal("%s"),'):format(tok[2], tok[1])
		Append(lines, def)
	end

	for _, tok in ipairs(keywords) do
		local def = ('\t[TokenType_%s] = str_literal("%s"),'):format(Capitalize(tok), tok)
		Append(lines, def)
	end

	Append(lines, '};')
	print(table.concat(lines, '\n'))
end

--- Matching
do
	local lines = {}
	local simpleTokenLines = {}
	local doubleTokenLines = {}

	local tokenCmp = function(a, b)
		if a[1] == b[1] then
			if #a[1] == #b[1] then
				return 0
			end
			return #a[1] < #b[1] and 1 or -1
		end
		return a[1] < b[1] and 1 or -1
	end

	Sort(delimiters, tokenCmp)

	local groups = {}
	for _, tok in ipairs(delimiters) do
		local key = tok[1]:sub(1,1)
		groups[key] = groups[key] or {}
		Append(groups[key], tok)
		assert(#tok[1] <= 2, 'Delimiters must be 2 chars at most')
	end

	local pushToken = function(tokType)
		return ('lexer_push_token(&lexer, TokenType_%s);'):format(tokType)
	end

	Append(lines, 'switch(c){')
	for prefix, tokens in pairs(groups) do
		local stmt = ("case '%s': "):format(prefix)
		Sort(tokens, tokenCmp)

		if #tokens == 1 then
			stmt = stmt .. pushToken(tokens[1][2])
			Append(simpleTokenLines, stmt)
		else
			local foundLengthOne = false
			for i, token in ipairs(tokens) do
				if i == 1 then
					stmt = stmt .. ("\n\t")
				else
					stmt = stmt .. ('\n\telse ')
				end

				if #token[1] == 1 then
					assert(not foundLengthOne, 'Abiguity detected')
					foundLengthOne = true
				else
					stmt = stmt .. ("if(lexer_advance_match(&lexer, '%s'))"):format(token[1]:sub(2,2))
				end
				stmt = stmt .. '{ ' .. pushToken(token[2]) ..' }'
			end
			Append(doubleTokenLines, stmt, 'break;')
		end
	end

	Append(lines, Unpack(simpleTokenLines))
	Append(lines, Unpack(doubleTokenLines))
	Append(lines, '}')

	for _, l in ipairs(lines) do print(l) end
end

