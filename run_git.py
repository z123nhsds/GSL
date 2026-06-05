import os
with open('/app/GSL/git_output.txt', 'w') as f:
    f.write(os.popen('git log -n 3').read())
