# Gource image

## Build
The docker image attached is provided to help create a gource video, without having to install locally all the dependencies.

To create the docker image simply run from within the docker folder:
 docker image build -t nna/gource:latest .

## Sample execution
```
cd /home/user/my/git/directory
docker run --rm -v ${PWD}:${PWD} -w ${PWD} -ti nna/gource --title BreadCrumbs --only-current-branch
```

## Additional parameters
```
  --title: the tile of the application [default: BreadCrumbs]
  --resolution: the target resolution [default: 1360x720]
  --log-format: the log format [default: custom]
  --seconds-per-day: the number of seconds for a day [default: 0.5]
  --auto-skip-seconds: auto skip seconds [default: 0.1]
  --elasticity: elasticity [default: 0.05]
  --hide: parameters to hide [default: filenames,dirnames,progress,mouse]
  --user-friction: user friction [default: 0.2]
  --output-framerate: frame rate [default: 25]
  --output-file: output file [default: video.mp4]
  --only-current-branch: [default: false]
```
