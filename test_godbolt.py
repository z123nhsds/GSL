import urllib.request
import json

code = """
#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
GSL_SUPPRESS(bounds .1)
void f() {}
"""

data = {
    "source": code,
    "compiler": "cg121",
    "options": {
        "userArguments": "-std=c++14 -fsyntax-only",
        "compilerOptions": {"skipAsm": True}
    },
    "lang": "c++"
}

req = urllib.request.Request(
    'https://godbolt.org/api/compiler/cg121/compile',
    data=json.dumps(data).encode('utf-8'),
    headers={'Content-Type': 'application/json', 'Accept': 'application/json'}
)

try:
    with urllib.request.urlopen(req) as response:
        result = json.loads(response.read().decode('utf-8'))
        print("WITH SPACE:")
        print("\n".join(msg['text'] for msg in result['stderr']))
except Exception as e:
    print(e)

code2 = """
#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
GSL_SUPPRESS(bounds.1)
void f() {}
"""
data["source"] = code2
req = urllib.request.Request(
    'https://godbolt.org/api/compiler/cg121/compile',
    data=json.dumps(data).encode('utf-8'),
    headers={'Content-Type': 'application/json', 'Accept': 'application/json'}
)
try:
    with urllib.request.urlopen(req) as response:
        result = json.loads(response.read().decode('utf-8'))
        print("\nWITHOUT SPACE:")
        print("\n".join(msg['text'] for msg in result['stderr']))
except Exception as e:
    print(e)
