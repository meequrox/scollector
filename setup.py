from setuptools import setup
import os

def get_version() -> str:
    v: str = "0.0.0"

    path = os.path.dirname(os.path.realpath(__file__))
    path = os.path.join(path, "scollector", "__version__")

    if os.path.isfile(path):
        fd = open(path)
        v = fd.read().strip()
        fd.close()

    return v

setup(
    name="scollector",
    version=get_version(),
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
