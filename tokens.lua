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

	{'>>', 'ShiftRight'},
	{'<<', 'ShiftLeft'},
	{'&', 'And'},
	{'|', 'Or'},
	{'~', 'Tilde'},

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

function Sort(list, cmpFn)
	cmpFn = cmpFn or function(a, b)
		if a == b then
			return 0
		end
		return a > b and -1 or 1
	end

	local function Partition(list, lo, hi, cmp)
		local pivot = list[lo]

		local i = lo - 1
		local j = hi + 1

		while true do
			repeat
				i = i + 1
			until cmp(list[i], pivot) >= 0

			repeat
				j = j - 1
			until cmp(list[j], pivot) <= 0

			if i >= j then
				return j
			end

			list[i], list[j] = list[j], list[i]
		end
	end

	local function QuickSort (list, lo, hi, cmp)
		if lo >= 1 and hi >= 1 and lo < hi then
			local p = Partition(list, lo, hi, cmp)
			QuickSort(list, lo, p, cmp)
			QuickSort(list, p + 1, hi, cmp)
		end
	end

	QuickSort(list, 1, #list, cmpFn)
end

--- Matching
do
	Sort(delimiters, function(a, b)
		if a[1] == b[1] then
			return 0
		end
		return a[1] < b[1] and 1 or -1
	end)

	for _, tok in ipairs(delimiters) do
		-- print(tok[1], tok[2])
	end
end
