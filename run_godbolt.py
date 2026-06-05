import urllib.request
import json
import sys

code = """
#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
GSL_SUPPRESS(f .4)
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
        with open("/app/GSL/godbolt_out.txt", "w") as f:
            for msg in result.get('stderr', []):
                f.write(msg.get('text', '') + "\n")
except Exception as e:
    with open("/app/GSL/godbolt_out.txt", "w") as f:
        f.write(str(e))
