///////////////////////////////////////////////////////////////////////////////////////////////////
/// main.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <LuaConnect\Exceptions\LuaException.h>
#include <LuaConnect\Function.h>
#include <LuaConnect\Table.h>
#include <LuaConnect\Type.h>
#include <LuaConnect\Userdata.h>
#include <LuaConnect\VM.h>


#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Lua Code
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string lua = R"(
local function print_globals()
    for n in pairs(_ENV) do print(n, type(_ENV[n])) end
end
local function print_message()
    print("print_message")
end

local function hellolua()
    print("Hello Lua")
end
local function hellocpp()
    HelloCpp()
end

local function call_throwexception()
    ThrowException()
end

local function create_constructorexceptionclass()
    local obj = ConstructorExceptionClass()
end

local function call_exceptionclass_staticthrowexception()
    ExceptionClass().StaticThrowException()
end
local function call_exceptionclass_throwexception()
    ExceptionClass():ThrowException()
end

local function handle_throwexception()
    if pcall(ThrowException) then
        error("No exception raised by ThrowException.")
    end
end

local function handle_constructorexceptionclass()
    if pcall(ConstructorExceptionClass) then
        error("No exception raised by ConstructorExceptionClass.")
    end
end
local function handle_exceptionclass()
    local obj = ExceptionClass()
    if pcall(obj.StaticThrowException) then
        error("No exception raised by ExceptionClass.StaticThrowException.")
    elseif pcall(obj.ThrowException, obj) then
        error("No exception raised by ExceptionClass.ThrowException.")
    end
end

local function throwexception()
    error("throwexception")
end

local function passobjects()
    PassingObjects(false, -9, 9, 0.9, "Passed by Lua", print_message, { val = "Set by Lua" }, PrintMessageClass())
    
    local b = ReturnBool()
    local i = ReturnLong()
    local ui = ReturnUnsignedInt()
    local n = ReturnDouble()
    local s = ReturnString()
    print(string.format("%s : %d : %d : %d : %s", b, i, ui, n, s))

    local f = ReturnFunction()
    f()

    local t = ReturnTable()
    print(t.val)

    local u = ReturnUserdata()
    u:PrintMessage()
end

return {
    print_globals = print_globals,
    print_message = print_message,

    hellolua = hellolua,
    hellocpp = hellocpp,
    
    create_constructorexceptionclass = create_constructorexceptionclass,

    call_throwexception = call_throwexception,

    call_exceptionclass_staticthrowexception = call_exceptionclass_staticthrowexception,
    call_exceptionclass_throwexception = call_exceptionclass_throwexception,
    
    handle_throwexception = handle_throwexception,
    
    handle_constructorexceptionclass = handle_constructorexceptionclass,
    handle_exceptionclass = handle_exceptionclass,

    throwexception = throwexception,

    passobjects = passobjects,
}
)";

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 1
///////////////////////////////////////////////////////////////////////////////////////////////////
void HelloCpp()
{
    std::cout << "Hello C++" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 2/3
///////////////////////////////////////////////////////////////////////////////////////////////////
void ThrowException()
{
    throw std::exception("::ThrowException()");
}

class ConstructorExceptionClass
{
public:
    ConstructorExceptionClass()
    {
        throw std::exception("ConstructorExceptionClass::ConstructorExceptionClass()");
    }
};
class ExceptionClass
{
public:
    static void StaticThrowException()
    {
        throw std::exception("ExceptionClass::StaticThrowException()");
    }

    void ThrowException()
    {
        throw std::exception("ExceptionClass::ThrowException()");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 5
///////////////////////////////////////////////////////////////////////////////////////////////////
void PrintMessage()
{
    std::cout << "::PrintMessage" << std::endl;
}

class PrintMessageClass
{
public:
    void PrintMessage()
    {
        std::cout << "PrintMessageClass::PrintMessage" << std::endl;
    }
};
class VMWrapper
{
public:
    LuaConnect::VM vm;
    VMWrapper(LuaConnect::VM vm) : vm(vm) { }
};

void PassingObjects(bool b, int i, unsigned int ui, double n, std::string s, LuaConnect::Function f, LuaConnect::Table t, LuaConnect::Userdata<PrintMessageClass> u)
{
    std::cout << b << " : " << i << " : " << ui << " : " << n << " : " << s << std::endl;

    f.Call<void>();
    
    std::cout << t.Get<std::string>("val") << std::endl;

    u.GetPointer()->PrintMessage();
}
bool ReturnBool()
{
    return true;
}
int ReturnLong()
{
    return -4;
}
unsigned int ReturnUnsignedInt()
{
    return 4;
}
double ReturnDouble()
{
    return 0.4;
}
std::string ReturnString()
{
    return "Returned from C++";
}
LuaConnect::Function ReturnFunction(LuaConnect::Userdata<VMWrapper> vm)
{
    return LuaConnect::Function::CreateFunction(vm.GetPointer()->vm, &PrintMessage);
}
LuaConnect::Table ReturnTable(LuaConnect::Userdata<VMWrapper> vm)
{
    LuaConnect::Table table(vm.GetPointer()->vm);
    table.Set("val", "Set by C++");

    return table;
}
LuaConnect::Userdata<PrintMessageClass> ReturnUserdata(LuaConnect::Userdata<VMWrapper> vm)
{
    return LuaConnect::Userdata<PrintMessageClass>(vm.GetPointer()->vm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 1 - Calling Lua from C++ and vice versa
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Test1()
{
    // Create VM
    LuaConnect::VM vm;

    // Load the Lua code
    LuaConnect::Function chunk = vm.LoadBuffer(lua, NULL);

    // Execute the chunk, retrieving the table returned from it
    LuaConnect::Table table = chunk.Call<LuaConnect::Table>();

    // Register HelloCpp
    vm.GetGlobalTable().Set("HelloCpp", LuaConnect::Function::CreateFunction(vm, &HelloCpp));

    // Execute the increment function of the table
    try
    {
        table.Call<void>("hellolua");
        table.Call<void>("hellocpp");
    }
    catch (const LuaConnect::LuaException& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 2 - Throwing exceptions in C++ code called by Lua
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Test2()
{
    // Create VM
    LuaConnect::VM vm;

    // Load the Lua code
    LuaConnect::Function chunk = vm.LoadBuffer(lua, NULL);

    // Execute the chunk, retrieving the table returned from it
    LuaConnect::Table table = chunk.Call<LuaConnect::Table>();

    // Register functions and types
    vm.GetGlobalTable().Set("ThrowException", LuaConnect::Function::CreateFunction(vm, &ThrowException));

    LuaConnect::Type<ConstructorExceptionClass>::RegisterType(vm, "ConstructorExceptionClass");
    LuaConnect::Type<ConstructorExceptionClass>::AddConstructor(vm, vm.GetGlobalTable());

    LuaConnect::Type<ExceptionClass>::RegisterType(vm, "ExceptionClass");
    LuaConnect::Type<ExceptionClass>::AddConstructor(vm, vm.GetGlobalTable());
    LuaConnect::Type<ExceptionClass>::AddFunction(vm, "StaticThrowException", &ExceptionClass::StaticThrowException);
    LuaConnect::Type<ExceptionClass>::AddFunction(vm, "ThrowException", &ExceptionClass::ThrowException);

    // Execute relevant Lua functions
    bool passed = true;
    try
    {
        table.Call<void>("call_throwexception");
        std::cout << "No exception raised by call_throwexception" << std::endl;

        passed = false;
    }
    catch (const LuaConnect::LuaException&)
    {
        std::cout << "Caught function exception" << std::endl;
    }

    try
    {
        table.Call<void>("create_constructorexceptionclass");
        std::cout << "No exception raised by create_constructorexceptionclass" << std::endl;

        passed = false;
    }
    catch (const LuaConnect::LuaException&)
    {
        std::cout << "Caught constructor exception" << std::endl;
    }

    try
    {
        table.Call<void>("call_exceptionclass_staticthrowexception");
        std::cout << "No exception raised by call_exceptionclass_staticthrowexception" << std::endl;

        passed = false;
    }
    catch (const LuaConnect::LuaException&)
    {
        std::cout << "Caught non-member method exception" << std::endl;
    }

    try
    {
        table.Call<void>("call_exceptionclass_throwexception");
        std::cout << "No exception raised by call_exceptionclass_throwexception" << std::endl;

        passed = false;
    }
    catch (const LuaConnect::LuaException&)
    {
        std::cout << "Caught member method exception" << std::endl;
    }

    return passed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 3 - Handling exceptions thrown in C++ code with Lua
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Test3()
{
    // Create VM
    LuaConnect::VM vm;

    // Load the Lua code
    LuaConnect::Function chunk = vm.LoadBuffer(lua, NULL);

    // Execute the chunk, retrieving the table returned from it
    LuaConnect::Table table = chunk.Call<LuaConnect::Table>();

    // Register functions and types
    vm.GetGlobalTable().Set("ThrowException", LuaConnect::Function::CreateFunction(vm, &ThrowException));

    LuaConnect::Type<ConstructorExceptionClass>::RegisterType(vm, "ConstructorExceptionClass");
    LuaConnect::Type<ConstructorExceptionClass>::AddConstructor(vm, vm.GetGlobalTable());

    LuaConnect::Type<ExceptionClass>::RegisterType(vm, "ExceptionClass");
    LuaConnect::Type<ExceptionClass>::AddConstructor(vm, vm.GetGlobalTable());
    LuaConnect::Type<ExceptionClass>::AddFunction(vm, "StaticThrowException", &ExceptionClass::StaticThrowException);
    LuaConnect::Type<ExceptionClass>::AddFunction(vm, "ThrowException", &ExceptionClass::ThrowException);

    // Execute relevant Lua methods
    bool passed = true;
    try
    {
        table.Call<void>("handle_throwexception");
        std::cout << "Handled function exception" << std::endl;
    }
    catch (const LuaConnect::LuaException& e)
    {
        std::cout << e.what() << std::endl;
        passed = false;
    }

    try
    {
        table.Call<void>("handle_constructorexceptionclass");
        std::cout << "Handled constructor exception" << std::endl;
    }
    catch (const LuaConnect::LuaException& e)
    {
        std::cout << e.what() << std::endl;
        passed = false;
    }

    try
    {
        table.Call<void>("handle_exceptionclass");
        std::cout << "Handled non-member and member exceptions" << std::endl;
    }
    catch (const LuaConnect::LuaException& e)
    {
        std::cout << e.what() << std::endl;
        passed = false;
    }

    return passed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 4 - Handling exceptions thrown in Lua code with C++
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Test4()
{
    // Create VM
    LuaConnect::VM vm;

    // Load the Lua code
    LuaConnect::Function chunk = vm.LoadBuffer(lua, NULL);

    // Execute the chunk, retrieving the table returned from it
    LuaConnect::Table table = chunk.Call<LuaConnect::Table>();

    // Execute relevant Lua methods
    try
    {
        table.Call<void>("throwexception");
        std::cout << "No exception raised by throwexception" << std::endl;

        return false;
    }
    catch (const LuaConnect::LuaException&)
    {
        std::cout << "Handled Lua exception" << std::endl;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Test 5 - Calling functions of all different signaturs from Lua
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Test5()
{
    // Create VM
    LuaConnect::VM vm;

    // Load the Lua code
    LuaConnect::Function chunk = vm.LoadBuffer(lua, NULL);

    // Execute the chunk, retrieving the table returned from it
    LuaConnect::Table table = chunk.Call<LuaConnect::Table>();

    // VMWrapper class to pass vm through Lua
    LuaConnect::Type<VMWrapper>::RegisterType(vm, "VMWrapper");
    LuaConnect::Userdata<VMWrapper> vmWrapper(vm, vm);

    // Register functions and types
    vm.GetGlobalTable().Set("PrintMessage", LuaConnect::Function::CreateFunction(vm, &PrintMessage));
    vm.GetGlobalTable().Set("PassingObjects", LuaConnect::Function::CreateFunction(vm, &PassingObjects));
    vm.GetGlobalTable().Set("ReturnBool", LuaConnect::Function::CreateFunction(vm, &ReturnBool));
    vm.GetGlobalTable().Set("ReturnLong", LuaConnect::Function::CreateFunction(vm, &ReturnLong));
    vm.GetGlobalTable().Set("ReturnUnsignedInt", LuaConnect::Function::CreateFunction(vm, &ReturnUnsignedInt));
    vm.GetGlobalTable().Set("ReturnDouble", LuaConnect::Function::CreateFunction(vm, &ReturnDouble));
    vm.GetGlobalTable().Set("ReturnString", LuaConnect::Function::CreateFunction(vm, &ReturnString));
    vm.GetGlobalTable().Set("ReturnFunction", LuaConnect::Function::CreateFunction(vm, &ReturnFunction, vmWrapper));
    vm.GetGlobalTable().Set("ReturnTable", LuaConnect::Function::CreateFunction(vm, &ReturnTable, vmWrapper));
    vm.GetGlobalTable().Set("ReturnUserdata", LuaConnect::Function::CreateFunction(vm, &ReturnUserdata, vmWrapper));

    LuaConnect::Type<PrintMessageClass>::RegisterType(vm, "PrintMessageClass");
    LuaConnect::Type<PrintMessageClass>::AddConstructor(vm, vm.GetGlobalTable());
    LuaConnect::Type<PrintMessageClass>::AddFunction(vm, "PrintMessage", &PrintMessageClass::PrintMessage);

    // Execute relevant Lua methods
    try
    {
        table.Call<void>("passobjects");
    }
    catch (const LuaConnect::LuaException& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

#include <vector>
std::vector<bool(*)()> m_tests =
{
    &Test1,
    &Test2,
    &Test3,
    &Test4,
    &Test5
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Entry Point
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    do
    {
        std::cout << "Run test: ";
        unsigned int val; std::cin >> val;

        if (val > m_tests.size())
        {
            std::cout << "Invalid test number..." << std::endl;
            continue;
        }
        else if (val == 0)
            break;

        std::cout << "---- BEGIN TEST #" << val << " ----" << std::endl;
        try
        {
            if (m_tests[val - 1]())
                std::cout << "Test Passed" << std::endl;
            else
                std::cout << "Test Failed" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Exception thrown by test: " << e.what() << std::endl;
            std::cout << "Test Failed" << std::endl;
        }
        catch (...)
        {
            std::cout << "Unknown exception thrown by test" << std::endl;
            std::cout << "Test Failed" << std::endl;
        }
        std::cout << "---- END TEST #" << val << " ----" << std::endl << std::endl;

    } while (true);
}