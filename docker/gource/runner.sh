#!/bin/bash
set -e

title=BreadCrumbs
resolution=1360x720
logFormat=custom
secondsPerDay=0.5
autoSkipSeconds=0.1
elasticity=0.05
hide=filenames,dirnames,progress,mouse
userFriction=0.2
frameRate=25
outputFile=video.mp4
currentBranch=false


gourceLogFile=/tmp/gource-combined-branch-logs.txt

mkdir -p /tmp/gource-logs/

function prepare_history() {

  if [ currentBranch ]; then
    gource --output-custom-log ${gourceLogFile} .
  else
    current_branch=`git branch --show-current`

    index=0
    for branch in `git branch -a | grep remotes | grep -v HEAD | grep -v master `; do
      echo Checking out branch ${branch}
      branch_name="$(echo ${branch} | cut -d'/' -f3)"
      echo ${branch_name}
      git checkout ${branch}
      gource --output-custom-log /tmp/gource-logs/${branch_name}.log .
   
      sed -i -r "s#(.+)\|#\1|/${branch_name}#" /tmp/gource-logs/${branch_name}.log
    done

    git checkout ${current_branch}

    cat /tmp/gource-logs/*.log | sort -n -u > ${gourceLogFile}
  fi
}

function generate() {
  xvfb-run -a -s '-screen 0 1360x720x16' gource \
    --title "${title}" \
    --log-format ${logFormat} \
    --stop-at-end \
    --seconds-per-day ${secondsPerDay} \
    --auto-skip-seconds ${autoSkipSeconds} \
    --elasticity ${elasticity} \
    -b 000000  --hide ${hide} \
    --user-friction ${userFriction} \
    -${resolution} \
    --output-ppm-stream - \
    --output-framerate ${frameRate} \
    ${gourceLogFile} | \
    ffmpeg \
      -analyzeduration 2147483647 \
      -probesize 2147483647 \
      -y \
      -r ${frameRate} \
      -f image2pipe \
      -vcodec ppm \
      -i - \
      -vcodec libx264 \
      -preset ultrafast \
      -crf 1 \
      -bf 0 \
      ${outputFile}
}

while [[ "$#" -gt 0 ]]; do
  case "$1" in
    --log-format)
      logFormat="$2"
      shift 2
      ;;
    --seconds-per-day)
      secondsPerDay=$2
      shift 2
      ;;
    --auto-skip-seconds)
      autoSkipSeconds=$2
      shift 2
      ;;
    --elasticity)
      elasticity=$2
      shift 2
      ;;
    --hide)
      hide=$2
      shift 2
      ;;
    --user-friction)
      userFriction=$2
      shift 2
      ;;
    --output-framerate)
      frameRate=$2
      shift 2
      ;;
    --output-file)
      outputFile=$2
      shift 2
      ;;
    --only-current-branch)
      currentBranch=true
      shift
      ;;
    --title)
      title="$2"
      shift 2
      ;;
    --resolution)
      resolution="$2"
      shift
      ;;
    --)
      shift
      break
      ;;
    *)
      echo "Unknown parameter $1"
      exit -1
      ;;
  esac
done

echo "title: ${title}"
echo "resolution: ${resolution}"
echo "log-format: ${logFormat}"
echo "seconds-per-day: ${secondsPerDay}"
echo "auto-skip-seconds: ${autoSkipSeconds}"
echo "elasticity: ${elasticity}"
echo "hide: ${hide}"
echo "user-friction: ${userFriction}"
echo "output-framerate: ${frameRate}"
echo "output-file: ${outputFile}"
echo "only-current-branch: ${currentBranch}"


prepare_history
generate
