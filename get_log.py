import subprocess
try:
    print(subprocess.check_output(['git', 'log', '--grep=remove space in GSL_SUPPRESS', '-p', '-1']).decode('utf-8'))
except Exception as e:
    print(e)
