# WasmEdge_error_reproduction

When using WasmEdge *Host Function* with *Threads proposal*, VM would meet some unknown errors.

But when using *Host Function* or *Threads proposal* respectively, there is no errors happen.

To repeat this result, you should have [WasmEdge](https://github.com/WasmEdge/WasmEdge) installed locally.

Now this is added as the [issue#2253](https://github.com/WasmEdge/WasmEdge/issues/2253) on WasmEdge/WasmEdge.

## *Host Function* + *Threads proposal* = error

Uncomment line 6 of `reproduction.cpp`.

Uncomment line 5 of `func.wat`.

Comment line 7 of `func.wat`.

Run `make` and `./reproduction`, then the error would be found:

`Error message: out of bounds memory access`

## *Host Function* = no errors

Comment line 6 of `reproduction.cpp`.

Uncomment line 5 of `func.wat`.

Comment line 7 of `func.wat`.

Run `make` and `./reproduction`, no errors happen.

## *Threads proposal* = no errors

Uncomment line 6 of `reproduction.cpp`.

Comment line 5 of `func.wat`.

Uncomment line 7 of `func.wat`.

Run `make` and `./reproduction`, no errors happen.
