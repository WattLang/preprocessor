MacroIdentifier = '@'
MacroStart      = "%["
MacroEnd        = '%]'

DefineMacro       = "define"
UndefineMacro     = "undefine"
IncludeMacro      = "include"
ForceIncludeMacro = "force_include"

DefineMacroValueSeperator         = ':'
IncludeMacroAbsolutePathIndicator = "/"

Defines   = {}
Undefines = {}
Includes  = {}

function PreprocessData(strData)

    local nDataLength = string.len(strData)
    local nLastIndex  = 1

    while nLastIndex do
        
        local nStart, _ = string.find(strData, MacroIdentifier, nLastIndex)
        local nValue, _ = string.find(strData, MacroStart,      nLastIndex)
        local nEnd  , _ = string.find(strData, MacroEnd,        nLastIndex)

        print(tostring(nStart) .. " " ..tostring(nValue) .. " " .. tostring(nEnd))

        if not nStart then
            nLastIndex = nil
            print("End processing")
            break
        else
            nLastIndex = nEnd + 1
        end

        print(string.sub(strData, nStart + 1, nValue - 1) .. " " .. string.sub(strData, nValue + 1, nEnd - 1))

        local strMacroType  = string.sub(strData, nStart + 1, nValue - 1)
        local strMacroValue = string.sub(strData, nValue + 1, nEnd - 1)

        if strMacroType == DefineMacro then
            local nDefineSeperator = string.find(strMacroValue, DefineMacroValueSeperator)
            if nDefineSeperator then
                Defines[string.sub(strMacroValue, 1, nDefineSeperator - 1)] = { string.sub(strMacroValue, nDefineSeperator), nStart }
                print("Defined: " .. string.sub(strMacroValue, 1, nDefineSeperator - 1) .. " with a value of " .. string.sub(strMacroValue, nDefineSeperator + 1) .. " at index " .. tostring(nStart - 1))
            else 
                assert("Error parsing define macro!\nExpected \':\' between define and value!")
                nLastIndex = nil
                break
            end
        elseif strMacroType == IncludeMacro then
            if strMacroValue[1] == IncludeMacroAbsolutePathIndicator then 
                print("Including absolute file: ")
            else
                print("Including relative file: ")
            end
        elseif strMacroType == ForceIncludeMacro then
            if strMacroValue[1] == IncludeMacroAbsolutePathIndicator then 
                print("Force including absolute file: ")
            else
                print("Force including relative file: ")
            end
        end




    end
    

    return strData
end