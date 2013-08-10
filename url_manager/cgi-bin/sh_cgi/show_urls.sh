#!/bin/bash

block_size=50
dump="../data/block_dump"
tmp="../data/tmp"

awk -F '\t' 'BEGIN{
    b_size = "'$block_size'";
}{
    url = $1;
    len = length(url) % b_size;
    print $1 "\t" $2 "\t" len;
}' $tmp > ${tmp}_show

echo "<table class="store-ui-table">
<thead>
    <tr>
        <th>Select</th>
        <th>Url</th>
        <th>Operation</th>
    </tr>
</thead>"

echo '<tbody class="allObjs">'

awk -F '\t' 'BEGIN{
}{
    md5 = $2;
    len = $3;
    print "<tr>";
    print "    <td><input type=\"checkbox\" value=\"" md5 "\"></td>"
    print "    <td><div align=\"left\"><a href=\"" $1 "\">" $1 "</a></div></td>";
    #print "    <td><a href=\"cgi-bin/sh_cgi/delete_url.cgi?line=" line "\" onclick=\"return confirmDel()\" target=\"_blank\">Delete</a></td>";
    print "    <td><a href=\"cgi-bin/sh_cgi/delete_url.cgi?len=" len "&md5=" md5 "\" onclick=\"return confirmDel()\" target=\"_blank\">Delete</a></td>";
    print "</tr>";
}' ${tmp}_show

echo '</tbody>'
echo '</table>'

#echo "Total `wc -l $urls | cut -d' ' -f1 ` urls."

