from setuptools import setup

with open(
        os.path.dirname(os.path.realpath(__file__)) +
        "/scollector/__version__") as reader:
    version = reader.read().strip()

setup(
    name="scollector",
    version=version,
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
