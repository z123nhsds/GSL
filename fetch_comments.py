import urllib.request
import json

req = urllib.request.Request(
    'https://api.github.com/repos/microsoft/GSL/issues/1248/comments',
    headers={'Accept': 'application/vnd.github.v3+json'}
)
try:
    with urllib.request.urlopen(req) as response:
        result = json.loads(response.read().decode('utf-8'))
        for comment in result:
            print(comment['user']['login'] + ": " + comment['body'])
except Exception as e:
    print(e)
