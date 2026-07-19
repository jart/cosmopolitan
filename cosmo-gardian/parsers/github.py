import urllib.request
import json
import urllib.parse

class GithubParser:
    def __init__(self, repo="jart/cosmopolitan"):
        self.repo = repo
        self.base_url = "https://api.github.com/search/issues"

    def search_issues(self, query_keywords, limit=3):
        # GitHub Search API for issues/PRs
        q = f"repo:{self.repo} {query_keywords}"
        url = f"{self.base_url}?q={urllib.parse.quote(q)}&per_page={limit}"
        
        req = urllib.request.Request(url)
        req.add_header('User-Agent', 'Cosmo-Guardian-V2')
        req.add_header('Accept', 'application/vnd.github.v3+json')
        
        results = []
        try:
            with urllib.request.urlopen(req, timeout=5) as response:
                data = json.loads(response.read().decode('utf-8'))
                for item in data.get('items', []):
                    results.append({
                        'title': item['title'],
                        'url': item['html_url'],
                        'number': item['number'],
                        'state': item['state'],
                        'type': 'PR' if 'pull_request' in item else 'Issue'
                    })
        except Exception as e:
            print(f"[Github Parser] Could not reach Github API: {e}")
        
        return results
