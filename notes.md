- Makefile doesn't have proper glob? Lol. `$(shell find raw)` as the target.
- `gcc -E` to remove all the `#define`s.
- First 8 bytes define a `u64` in MSB to LSB order.
- Every other byte is taken individually. Only directly affects the new `x` value when `lo` and `hi` match in their top 8 bits.

## rot.bin

```js
==== LIST 0 ====

0000: '0101000'
0001: '0111000'
000: '00100'
0101010100100: '0101001000101'
0101011100100: '0111001000101'
0111010100100: '0101001000111'
0111011100100: '0111001000111'
010100100: '110100101'
011100100: '111100101'
0010101: '0100101'
0010111: '1100101'
00101: '00100'
00100: '0011'
11010011: '00110'
11110011: '00111'
0011: ''
```

In specific:

1. Prepend `000` to the input string
2. Apply the above replacements until fixed point. In each iteration, find the leftmost position that matches one of the strings (and apply the leftmost string that matches)

For example, an input of `1` gives an output of `1` by the following sequence:

1. Prepend `000` to get `0001`
1. Replace `0001` with `0111000` to get `0111000`. Note the `000` rule does not apply since it's later in the list than the `0001` rule.
1. Replace `000` with `00100` to get `011100100`. Note this replacement starts in (0-indexed) position 4. That's okay since all the replacements do not apply, e.g. there's no rule `0111` or `111001` or `1100100`.
1. Replace `011100100` with `111100101` to get `111100101`.

It keeps going:

```
111100101
Applying 00101: '00100'
111100100
Applying 00100: '0011'
11110011
Applying 11110011: '00111'
00111
Applying 0011: ''
1
```

Finally terminates at 1.

Interestingly, if the input string has no consecutive zeros, then this reverses the input string.

JavaScript translation in [programs/rot.js](./programs/rot.js)

## mix.bin

```js
==== LIST 0 ====

00010: (swap 1 (slice 0))

==== LIST 1 ====

10: '1'
```

<style type="text/css">
    ol ol { list-style-type: upper-alpha; }
    ol ol ol { list-style-type: lower-roman; }
</style>

Example input `101010`

1. Prepend 000 to get `000101010`. Now eval `000101010` at list 0.
1. Look through list 0: It finds `00010` in position 0. The remainder of the input is `1010`
   1. Compute `(swap 1 (slice 0))` on `1010`
   1. Switch to list 1. Eval `(slice 0)` on `1010`.
   1. `slice 0` is identity function, so the op finishes and returns `1010`.
   1. We're in List 1 now, so apply the List 1 rule: replace `10` with `1`. One replacement gives `110`.
   1. The second replacement gives `11`.
   1. This is a fixed point (there are no more `10`), so return `11`.
1. Replace the initial `00010` with `11`. Now the string is `111010`
1. We're in List 0, and there are no more `00010`, so this is a fixed point. Return `111010`.

Summary. What mix.bin does is:

If the string looks like `A00010B`, where `A` does not contain `00010`, then replace it with `ACB`, where `C` is `B` with all `10` replaced with `1` recursively. Repeat until fixed point.
