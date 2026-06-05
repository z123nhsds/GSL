import subprocess
try:
    result = subprocess.run(['git', 'log', '-p', '-S', 'space'], capture_output=True, text=True)
    print(result.stdout[:2000])
except Exception as e:
    print(e)
