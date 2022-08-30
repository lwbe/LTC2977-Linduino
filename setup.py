#!/usr/bin/env python

from setuptools import setup, find_packages

setup(
      name='LTC2977_duino',
      version='0.2.2',
      description='Module to interact with LTM2877 through Linduino',
      author='Lorenzo Bernardi',
      author_email='bernardi@llr.in2p3.fr',
      #url='https://www.python.org/sigs/distutils-sig/',
      install_requires=['pyserial'],
      license="MIT",
      packages=find_packages(),
      package_data={'LTC2977_duino': ['ltc2977.desc']},
     )
