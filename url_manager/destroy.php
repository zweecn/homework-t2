<?php 
session_start();  
if($_SESSION['user']=="admin" && $_SESSION['pass']=="admin"){
       echo "<div align=\"center\">Logout succeed.<br />";
       session_destroy();  
	   echo "<meta http-equiv=\"refresh\" content=\"3;url=index.php\">Return to login page...</div>";
   }else{
       echo "<div align=\"center\">Permission denied. <br />";
	   echo "<meta http-equiv=\"refresh\" content=\"3;url=140.php\">Return to login page...</div>";
   }
?>
