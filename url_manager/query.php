<?php session_start();
if($_SESSION['user']=="admin" && $_SESSION['pass']=="admin"){
    $a = file('./cgi-bin/data/url_cnt');
    foreach($a as $line => $total_urls){
    }
?>


<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=gb2312">
        <title>URL Manager</title>
        <script src="js/url_manager.js"  type = "text/javascript" ></script>
        <link rel="stylesheet" href="query.css" type="text/css"/>
    </head>


    <body onload="loadAllObjs()" >

 		<p>You are already logged. <a href="./destroy.php">Logout</a></p>
        <div class="input-box-ui">
            <form name="user_info" method="post" action="">
                <table class="box-ui">
                    <tr><td><input class="input-box" type="text" name="q" ></td><td><input class="op-name" type="button" value="Search" onclick="searchUrl()"></td></tr> 
                </table>
            </form>
            <div id="msg"></div>
        </div>
        <div class="input-box-ui">
            <form name="addurl_form" method="post" action="">
                <table class="box-ui">
                    <tr><td><input class="input-box" type="text" name="url" ></td><td><input class="op-name" type="button" value="Add" onclick="addUrl()"></td></tr>
                </table>
            </form>
            <div id="addurl_msg"></div>
        </div>

        <div class="urls-show-ui">
            <div id="allObjs" ></div>
            <form name="show_obj">
                Total of <?php echo $total_urls; ?> urls.
                Per page has <input type="text" value="20" name="per_page"> items.
                Current page is <input type="text" name="cur_page" value="1">
                <input type="button" value="Go" onclick="loadAllObjs()">
            </form>
        </div>
    </body>

</html>

<?php }else{
header("location:login_ok.php");
}
?>
