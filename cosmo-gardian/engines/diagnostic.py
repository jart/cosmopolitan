import json
import re
import os
from engines.math_engine import MathEngine

class DiagnosticEngine:
    def __init__(self, knowledge_path):
        with open(knowledge_path, 'r', encoding='utf-8') as f:
            self.kb = json.load(f)
        self.math = MathEngine()

    def analyze_compiler_error(self, stderr_output):
        diagnostics = []
        lines = stderr_output.splitlines()
        for line in lines:
            for err_rule in self.kb.get('compiler_errors', []):
                match = re.search(err_rule['pattern'], line)
                if match:
                    extracted = match.groups()
                    explanation = err_rule['explanation'].format(*extracted)
                    suggestion = err_rule['suggestion'].format(*extracted)
                    
                    if err_rule.get('trigger') == 'overflow':
                        math_demo = self.math.check_addition(2147483647, 1, 'int32')
                        explanation += f"\n\n      [Math Engine Activated]\n      {math_demo['explanation']}\n      Example: {math_demo['operation']} silently wraps to {math_demo['interpreted_c_value']}!"
                    
                    # Avoid duplicates if the same error is printed multiple times
                    diag = {
                        "raw_error": line.strip(),
                        "explanation": explanation,
                        "suggestion": suggestion
                    }
                    if diag not in diagnostics:
                        diagnostics.append(diag)
        return diagnostics
