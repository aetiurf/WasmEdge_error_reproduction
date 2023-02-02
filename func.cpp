#define WASM_EXPORT(name) __attribute__((export_name(name)))
#define WASM_IMPORT(mod, name) __attribute__((import_module(mod), import_name(name)))

WASM_IMPORT("server", "mul")
int mul(int length);

WASM_EXPORT("func")
int func(int n)
{
    return mul(n);
}