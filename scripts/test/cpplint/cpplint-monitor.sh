#!/usr/bin/env sh

REMOTE_REPO="http://google-styleguide.googlecode.com/svn/"
LOCAL_REPO="../../ant/tools/google-styleguide/google-styleguide-read-only"

REMOTE_REV=`svn info $REMOTE_REPO | grep '^Revision:' | awk '{print $2}'`
LOCAL_REV=`svn info $LOCAL_REPO | grep '^Revision:' | awk '{print $2}'`

echo "remote revision: $REMOTE_REV"
echo "local revision: $LOCAL_REV"

if [ $REMOTE_REV -eq $LOCAL_REV ]; then
  echo 'ccplint up to date'
  exit 0;
else
  echo 'update local cpplint repo...'
  cd $LOCAL_REPO
  svn up
  echo 'done'
  exit 1
fi

