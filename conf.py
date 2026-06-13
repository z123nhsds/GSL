# conf.py
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx_autodoc_typehints',
]

# 解决返回类型重复的精确配置
napoleon_use_rtype = False
typehints_document_rtype = True
typehints_use_rtype = False

# 修复 threading.local 错误链接为 __thread__local 的问题
typehints_fixup_module_name = {
    '__thread__local': 'threading.local',
    '__thread._local': 'threading.local',
    '_thread._local': 'threading.local'
}
