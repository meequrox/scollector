from setuptools import setup

setup(
    name="scollector",
    version="2023.4.10",
    packages=["scollector"],
    url="https://github.com/meequrox/scollector",
    license="GPLv3",
    author="meequrox",
    author_email="",
    description="Collects tracks you haven't heard yet from SoundCloud charts",
    install_requires=["yt_dlp"],
    entry_points={
        "console_scripts": ["scollector=scollector.command_line:main"]
    })
