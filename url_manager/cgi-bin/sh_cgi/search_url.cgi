#!/bin/sh


echo "Content-type: text/html"
echo
echo

if [ "$REQUEST_METHOD" = "POST" ] ; then
    QUERY_STRING=`cat -`
fi

query=`echo $QUERY_STRING | cut -d'=' -f2-`
len=`echo $query | awk '{print length($1);}'`

md5=`echo $query | md5sum | cut -d' ' -f1`
last2char=${md5: -2}

block_size=50
dump="../data/block_dump"
f=${dump}/$(($len % $block_size))
ff=${f}/dump_d_${last2char}
fd=${f}/dump


line=`fgrep -n $md5 $ff | cut -d':' -f1`
if [ ! -z $line ]; then

    echo "<table class="store-ui-table">
    <thead>
    <tr>
    <th>Select</th>
    <th>Url</th>
    <th>MD5</th>
    <th>Operation</th>
    </tr>
    </thead>"

    echo '<tbody class="searchObjs">'

    echo $query | awk -F '\t' 'BEGIN{
    }{
        md5 = "'$md5'"
        len = "'$len'"
        print "<tr>";
        print "    <td><input type=\"checkbox\" value=\"" md5 "\"></td>"
        print "    <td><a href=\"" $1 "\">" $1 "</a></td>";
        print "    <td>" md5 "</td>"
        print "    <td><a href=\"cgi-bin/sh_cgi/delete_url.cgi?len=" len "&md5=" md5 "\" onclick=\"return confirmDel()\" target=\"_blank\">Delete</a></td>";
        print "</tr>";
        i++;
    }'

    echo '</tbody>'
    echo '</table>'

else
    echo "<p>URL does not exist.</p><p>Query:"$query"<br/>MD5:"$md5"</p>"
fi

