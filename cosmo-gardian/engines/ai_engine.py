import os
import json
import urllib.request
from parsers.github import GithubParser

class AIEngine:
    def __init__(self):
        self.github = GithubParser()
        self.api_key = os.environ.get("LLM_API_KEY")
        self.api_url = os.environ.get("LLM_API_URL", f"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key={self.api_key}")

    def call_llm(self, prompt):
        url = self.api_url
        data = {
            "contents": [{"parts": [{"text": prompt}]}],
            "systemInstruction": {
                "parts": [{"text": "You are Cosmo Guardian, an AI expert on Justin Tunney's Cosmopolitan Libc. Analyze the compilation errors and Github issues provided, and give a concise, accurate diagnosis and solution in French."}]
            }
        }
        
        req = urllib.request.Request(url, data=json.dumps(data).encode('utf-8'), method='POST')
        req.add_header('Content-Type', 'application/json')
        
        try:
            with urllib.request.urlopen(req, timeout=15) as response:
                result = json.loads(response.read().decode('utf-8'))
                return result['candidates'][0]['content']['parts'][0]['text']
        except Exception as e:
            return f"[API Error] Failed to contact Gemini API: {e}"

    def analyze_issue(self, error_summary, keywords):
        print(f"[AI Engine] Searching GitHub memory for '{keywords}'...")
        issues = self.github.search_issues(keywords)
        
        print("[AI Engine] Synthesizing context...\n")
        
        if not issues:
            context_text = "Aucune issue trouvée sur Github."
        else:
            context_text = "Issues Github pertinentes trouvées:\n"
            for i in issues:
                context_text += f"- #{i['number']} ({i['state']}): {i['title']} - {i['url']}\n"
        
        if self.api_key:
            print("[AI Engine] LLM API Key found. Querying the LLM...")
            prompt = f"Erreur de compilation:\n{error_summary}\n\nContexte Github:\n{context_text}\n\nAgis comme l'expert Cosmopolitan et explique le problème et sa solution probable."
            return self.call_llm(prompt)
        else:
            print("[AI Engine] No LLM_API_KEY found. Falling back to Mock Synthesis...")
            if not issues:
                return "L'IA n'a trouvé aucune Issue ou PR correspondante sur le dépôt Cosmopolitan. Il semble que ce problème soit inédit ou très spécifique à ton environnement."
            
            top_issue = issues[0]
            synthesis = f"D'après l'historique du projet Cosmopolitan, cette erreur ('{error_summary}') ressemble beaucoup au problème discuté dans la **{top_issue['type']} #{top_issue['number']}** : *\"{top_issue['title']}\"*.\n"
            
            if top_issue['state'] == 'closed':
                synthesis += "Bonne nouvelle : Justin Tunney ou la communauté a déjà traité et résolu ce problème par le passé. "
            else:
                synthesis += "Attention : Ce problème semble être actuellement ouvert ou en cours de discussion sur le dépôt. "
                
            synthesis += f"\n\nLien vers la discussion : {top_issue['url']}\n"
            
            if len(issues) > 1:
                synthesis += "\nAutres références pertinentes trouvées par l'IA :\n"
                for issue in issues[1:]:
                    synthesis += f"- #{issue['number']} : {issue['title']}\n"
                    
            return synthesis
