<?php

const class1_php = <<<'EOF'
<?php
class Class1{
    function __construct($myarg){
        echo "Class1 initializing" . PHP_EOL;
        $this->myarg = $myarg;
    }
    function run(){
        usleep(1000);
        echo "Class1 run() is called" . PHP_EOL;
    }
}
EOF;

const class2_php = <<<'EOF'
<?php
require_once __DIR__ . "/cororun.php";
require_once __DIR__ . "/autoload.php";
class Class2{
    function __construct($myarg){
        echo "Class2 initializing" . PHP_EOL;
    }
    function run(){
        usleep(1000);
        echo "Class2 run() is called" . PHP_EOL;
        cororun(function(){
            $this->class1 = new Class1("c1");
            usleep(1000);
        });
    }
}
EOF;

const autoload_php = <<<'EOF'
<?php
spl_autoload_register(static function ($class){
    require __DIR__ . "/" . $class . ".php";
});
EOF;

const cororun_php = <<<'EOF'
<?php
function cororun($x){
    if(extension_loaded("Swow")){
        usleep(1000);
        \Swow\Coroutine::run($x);
    }else{
        $x();
    }
}
EOF;


const run_head = <<<'EOF'
<?php
require_once __DIR__ . "/cororun.php";
echo "on run.php" . PHP_EOL;
EOF;
const test_autoload_str = <<<'EOF'
// include with spl autoloader
require_once __DIR__ . "/autoload.php";
cororun(function(){
    $x = new Class1("a");
    $x->run();
});
cororun(function(){
    $x = new Class2("b");
    $x->run();
});
EOF;
const test_include_str = <<<'EOF'
// include with include_once
cororun(function(){
    include_once __DIR__ . "/Class1.php";
    $x = new Class1("a");
    $x->run();
});
cororun(function(){
    include_once __DIR__ . "/Class2.php";
    $x = new Class2("b");
    $x->run();
});
EOF;
const test_require_str = <<<'EOF'
// include with require_once
cororun(function(){
    require_once __DIR__ . "/Class1.php";
    $x = new Class1("a");
    $x->run();
});
cororun(function(){
    require_once __DIR__ . "/Class2.php";
    $x = new Class2("b");
    $x->run();
});
EOF;

const TEST_AUTOLOAD = 0;
const TEST_INCLUDE = 0;
const TEST_REQUIRE = 0;

function build_phar($pharFile = __DIR__ . "/phartest.phar", $test_type = TEST_AUTOLOAD){
    if (file_exists($pharFile)){
        unlink($pharFile);
    }
    $phar = new \Phar($pharFile);
    switch($test_type){
        case TEST_AUTOLOAD:
            $phar->addFromString("autoload.php", autoload_php);
            $phar->addFromString("run.php", run_head . test_autoload_str);
            break;
        case TEST_INCLUDE:
            $phar->addFromString("run.php", run_head . test_include_str);
            break;
        case TEST_REQUIRE:
            $phar->addFromString("run.php", run_head . test_require_str);
            break;
        default:
            throw new Exception("no such type");

    }
    $phar->addFromString("cororun.php", cororun_php);
    $phar->addFromString("Class1.php", class1_php);
    $phar->addFromString("Class2.php", class2_php);
    $phar->compressFiles (\Phar::GZ);
    $phar->stopBuffering();
    $phar->setStub($phar->createDefaultStub("run.php"));
}
