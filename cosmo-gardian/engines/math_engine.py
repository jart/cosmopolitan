class MathEngine:
    def __init__(self):
        self.limits = {
            'int8': (-128, 127, 0xFF),
            'uint8': (0, 255, 0xFF),
            'int16': (-32768, 32767, 0xFFFF),
            'uint16': (0, 65535, 0xFFFF),
            'int32': (-2147483648, 2147483647, 0xFFFFFFFF),
            'uint32': (0, 4294967295, 0xFFFFFFFF),
            'int64': (-9223372036854775808, 9223372036854775807, 0xFFFFFFFFFFFFFFFF),
            'uint64': (0, 18446744073709551615, 0xFFFFFFFFFFFFFFFF)
        }

    def check_addition(self, a, b, dtype='int32'):
        if dtype not in self.limits:
            return {"error": f"Unknown data type {dtype}"}

        min_val, max_val, mask = self.limits[dtype]
        
        # Infinite precision addition
        real_sum = a + b
        
        # Wrapped (modulo) addition like C does natively
        wrapped_sum = real_sum & mask
        
        # For signed types, we need to sign-extend the wrapped sum to show how C interprets it
        interpreted_sum = wrapped_sum
        if dtype.startswith('int'):
            sign_bit = (mask >> 1) + 1
            if wrapped_sum & sign_bit:
                interpreted_sum = wrapped_sum - mask - 1

        overflow = real_sum > max_val or real_sum < min_val

        explanation = ""
        if overflow:
            if dtype.startswith('uint'):
                explanation = "Unsigned integer wrap-around occurred. C standard guarantees modulo arithmetic."
            else:
                explanation = "Signed integer overflow occurred! This is UNDEFINED BEHAVIOR in C. It may cause security vulnerabilities or crashes."
        else:
            explanation = "Safe addition."

        return {
            "operation": f"{a} + {b} in {dtype}",
            "real_sum": real_sum,
            "wrapped_sum_hex": hex(wrapped_sum),
            "interpreted_c_value": interpreted_sum,
            "overflow_occurred": overflow,
            "min_limit": min_val,
            "max_limit": max_val,
            "explanation": explanation
        }
        
    def explain_alignment(self, size, required_alignment=16):
        padding = (required_alignment - (size % required_alignment)) % required_alignment
        aligned_size = size + padding
        
        explanation = f"A structure or buffer of size {size} bytes requires {padding} bytes of padding to reach a {required_alignment}-byte boundary.\n"
        if required_alignment == 16:
            explanation += "Note: 16-byte alignment is critical in Cosmopolitan for SSE/AVX instructions and ABI compliance across OS kernels."
            
        return {
            "original_size": size,
            "required_alignment": required_alignment,
            "padding_needed": padding,
            "aligned_size": aligned_size,
            "explanation": explanation
        }
