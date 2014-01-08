env = Environment(CPPXX='g++', CPPFLAGS='-std=c++11 -Wall -Wextra -pedantic -pedantic-errors -Ofast --fast-math')
env.Append(LIBS=['boost_program_options', 'boost_graph'])
Default(env.Program('graph_analyzer',
    ['main.cpp',
     'program_options_parser.cpp',
     'graph_analyzer.cpp']))
