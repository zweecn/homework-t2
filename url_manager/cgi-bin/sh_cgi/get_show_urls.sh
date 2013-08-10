#!/bin/bash

# show curr_page th page, per page has n items.

curr_page=$1
per_page=$2

line_begin=$((${per_page} * $((${curr_page} - 1)) + 1))
#line_end=$(($line_begin + $per_page - 1))

##echo $line_begin
##echo $line_end


query=`echo $QUERY_STRING | cut -d'=' -f2-`
len=`echo $query | awk '{print length($1);}'`

md5=`echo $query | md5sum | cut -d' ' -f1`
last2char=${md5: -2}

block_size=50
dump="../data/block_dump"
tmp="../data/tmp"
rm -rf $tmp
need_print=$per_page
for ((i=0; i<$block_size; i++))
do
    f=${dump}/$(($i % $block_size))
    for f_name in `ls ${f}/dump_d_*`
    do
        cur_line=`wc -l $f_name | cut -d' ' -f1`
        offset=$(($line_begin - $cur_line))
        if [ "$offset" -gt 0 ]; then
            line_begin=$offset
        else
            awk 'BEGIN{
                i = "'$line_begin'";
                j = "'$need_print'"
            }{
                if (i > 0) {
                    i--;
                    next;
                } else if(j > 0) {
                    print;
                    j--;
                }
            }' $f_name >> $tmp
            line_begin=0
            already_print=`wc -l $tmp | cut -d' ' -f1`
            need_print=$(($per_page - $already_print))
            if [ "$need_print" -le 0 ]; then
                exit
            fi
        fi
    done
done

#echo "<table class="store-ui-table">
#<thead>
#    <tr>
#        <th>Select</th>
#        <th>Url</th>
#        <th>Operation</th>
#    </tr>
#</thead>"
#
#echo '<tbody class="allObjs">'
#
#awk -F '\t' 'BEGIN{
#    i = 0;
#}{
#    line = "'$line_begin'" + i;
#    print "<tr>";
#    print "    <td><input type=\"checkbox\" value=\"" line "\"></td>"
#    print "    <td><div align=\"left\"><a href=\"" $1 "\">" $1 "</a></div></td>";
#    #print "    <td><a href=\"\">Delete</a></td>";
#    #print "    <td><form name=\"delete_form\" action=\"#\" method=\"post\"><p> <button name=\"delete_line\" value=\"" line "\" onclick=\"deleteUrlConfirm()\">Delete</button> </p></form></td>";
#    print "    <td><a href=\"cgi-bin/sh_cgi/delete_url.cgi?line=" line "\" onclick=\"return confirmDel()\" target=\"_blank\">Delete</a></td>";
#    print "</tr>";
#    i++;
#}' $tmp
#
#echo '</tbody>'
#echo '</table>'
#
##echo "Total `wc -l $urls | cut -d' ' -f1 ` urls."
#
