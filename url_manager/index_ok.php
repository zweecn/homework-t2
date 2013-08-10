<?php session_start();  
      $user=$_POST['user'];
      $pass=$_POST['pass']; 
if($user=="admin" && $pass=="admin"){
   echo "<div align=\"center\">Login succeed.<br />";
   $_SESSION['user'] = 'admin';
   $_SESSION['pass'] = 'admin';
   echo "<meta http-equiv=\"refresh\" content=\"3;url=query.php\"> Please wait 3 seconds to jump to the home page...</div>";
   }else{
   echo "<div align=\"center\">Login failed.<br />";
   echo "<meta http-equiv=\"refresh\" content=\"3;url=index.php\"> Return to login page...</div>";
   }
?>
