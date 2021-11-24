from setuptools import Extension, setup
from Cython.Build import cythonize
import os

ext_modules = [
    Extension("Planner",
              sources=["Planner.pyx"],
              libraries=["TrafficMonitor","Cognition","Guidance","TrajectoryManager","GeofenceMonitor","Merger","Utils"], 
              include_dirs=[os.path.join(os.environ['ICAROUS_HOME'],'Modules'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','Cognition'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','Guidance'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','TrafficMonitor'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','Merger'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','TrajectoryManager'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','TrajectoryManager','DubinsPlanner'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','Interfaces'),
                            os.path.join(os.environ['ICAROUS_HOME'],'Modules','Core','Utils')],
              library_dirs = [os.path.join(os.environ['ICAROUS_HOME'],'Modules','lib')],
              language = 'c++11',
              #extra_compile_args = ['-stdlib=libc++'],
             )
]

setup(name="Planner", ext_modules=cythonize(ext_modules,compiler_directives={'language_level' : "3"}))
