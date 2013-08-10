#!/bin/sh

echo "Content-type: text/html"
echo
echo

if [ "$REQUEST_METHOD" = "POST" ] ; then
    QUERY_STRING=`cat -`
fi

per_page=`echo $QUERY_STRING | cut -d'&' -f1 | cut -d'=' -f2`
cur_page=`echo $QUERY_STRING | cut -d'&' -f2 | cut -d'=' -f2`


./get_show_urls.sh $cur_page $per_page 
./show_urls.sh

