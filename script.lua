MacroIdentifier = '@'
MacroStart      = "%["
MacroEnd        = '%]'

DefineMacro       = "define"
UndefineMacro     = "undefine"
IncludeMacro      = "include"
ForceIncludeMacro = "force_include"

DefineMacroValueSeperator         = ':'
IncludeMacroAbsolutePathIndicator = '/'

Defines   = {}
Undefines = {}
Includes  = {}

function tablelength(T)
    local count = 0
    for _ in pairs(T) do count = count + 1 end
    return count
end

function PreprocessData(strName, strData)

    print("Processing file: \"" .. strName .. "\"")

    strWorkingData = strData;

    local nDataLength = string.len(strData)
    local nLastIndex  = 1

    while nLastIndex do
        
        local nStart, _ = string.find(strWorkingData, MacroIdentifier, nLastIndex)
        local nValue, _ = string.find(strWorkingData, MacroStart,      nLastIndex)
        local nEnd  , _ = string.find(strWorkingData, MacroEnd,        nLastIndex)

        print(tostring(nStart) .. " " ..tostring(nValue) .. " " .. tostring(nEnd))

        if not nStart then
            nLastIndex = nil
            print("End processing")
            break
        else
            nLastIndex = nEnd + 1
        end

        print(string.sub(strWorkingData, nStart + 1, nValue - 1) .. " " .. string.sub(strWorkingData, nValue + 1, nEnd - 1))

        local strMacroType  = string.sub(strWorkingData, nStart + 1, nValue - 1)
        local strMacroValue = string.sub(strWorkingData, nValue + 1, nEnd   - 1)

        StringSpaceReplace(strWorkingData, nStart, nEnd)

        if strMacroType == DefineMacro then
            local nDefineSeperator = string.find(strMacroValue, DefineMacroValueSeperator)
            if nDefineSeperator then
                Defines[string.sub(strMacroValue, 1, nDefineSeperator - 1)] = { string.sub(strMacroValue, nDefineSeperator + 1), nStart }
                print("Defined: " .. string.sub(strMacroValue, 1, nDefineSeperator - 1) .. " with a value of " .. string.sub(strMacroValue, nDefineSeperator + 1) .. " at index " .. tostring(nStart - 1))
            else 
                assert("Error parsing define macro!\nExpected \':\' between define and value!")
                nLastIndex = nil
                break
            end


        elseif strMacroType == UndefineMacro then
            Undefines[strMacroValue] = nStart
            print("Undefined: " .. strMacroValue .. " at index " .. tostring(nStart - 1))


        elseif strMacroType == IncludeMacro then
            local strFileName = nil
            if string.byte(strMacroValue) == string.byte(IncludeMacroAbsolutePathIndicator) then 
                strFileName = string.sub(strMacroValue, 2)
                print("Including absolute file: " .. strFileName)
            else
                local nLastPathID = string.find(strName, "/.-$", 1)
                strFileName = string.sub(strName, 1, nLastPathID) .. strMacroValue
                print("Including relative file: " .. strFileName)
            end


            bAlreadyIncluded = false

            for k, v in pairs(Includes) do
                if v == strFileName then
                    bAlreadyIncluded = true
                    print("File: \"" .. strFileName .. "\" already included!")
                end
            end

            if not bAlreadyIncluded then
                Includes[tablelength(Includes)] = strFileName

                local f = assert(io.open(strFileName, "r"))
                local strFileContents = f:read("*all")
                f:close()

                StringInsertFormat(strWorkingData, nStart)
                strWorkingData = string.format(strWorkingData, strFileContents)
            end


        elseif strMacroType == ForceIncludeMacro then
            local strFileName = nil
            if string.byte(strMacroValue) == string.byte(IncludeMacroAbsolutePathIndicator) then 
                strFileName = string.sub(strMacroValue, 2)
                print("Force including absolute file: " .. strFileName)
            else
                local nLastPathID = string.find(strName, "/.-$", 1)
                strFileName = string.sub(strName, 1, nLastPathID) .. strMacroValue
                print("Force including relative file: " .. strFileName)
            end

            bAlreadyIncluded = false

            for k, v in pairs(Includes) do
                if v == strFileName then
                    bAlreadyIncluded = true
                end
            end

            if not bAlreadyIncluded then Includes[tablelength(Includes)] = strFileName end

            local f = assert(io.open(strFileName, "r"))
            local strFileContents = f:read("*all")
            f:close()

            StringInsertFormat(strWorkingData, nStart)
            strWorkingData = string.format(strWorkingData, strFileContents)
        end

    end

    print("Start define processing")

    for k, v in pairs(Defines) do

        print(k .. "  :  " .. v[1] .. "  :  " .. tostring(v[2]))

        local nLastIndex = 1

        while nLastIndex do
            local nCurrentIndex = string.find(strWorkingData, k, nLastIndex)
            if not nCurrentIndex then
                nLastIndex = nil
                print("End define processing")
                break
            else
                nLastIndex = nCurrentIndex + 1
            end
            if not Undefines[k] then
                strWorkingData = string.gsub(strWorkingData, k, v[1], 1)
                print("Replaced " .. k .. " with " .. v[1])
            elseif nCurrentIndex < Undefines[k] then
                strWorkingData = string.gsub(strWorkingData, k, v[1], 1)
                print("Replaced " .. k .. " with " .. v[1])
            end
        end

    end
    

    return strWorkingData
end

function PrintLines(str)
    for line in string.gmatch(str,'[^\r\n]+') do print(line) end
end