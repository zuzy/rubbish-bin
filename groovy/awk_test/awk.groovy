class Example {

    static void main(String[] args) {
        // Using a simple println statement to print output to the console
        println('Hello World')
        def sout = new StringBuilder(), serr = new StringBuilder()
        def proc = "echo 'Ninja;/home/tong.zhu/projects/tops_tmp3/tops;/home/tong.zhu/projects/tops_tmp3/cmake_build;-DINCLUDE_CLOUD=ON -DPACKAGE_GENERATOR=DEB -DUPLOAD_BAZEL_REMOTE_CACHE=ON -DINCLUDE_GDB=OFF -DOFFICIAL=OFF -DINCLUDE_DSOPT=OFF;' | grep Ninja"
        // log.info(proc)
        // def proc = "echo 'Ninja;/home/tong.zhu/projects/tops_tmp3/tops;/home/tong.zhu/projects/tops_tmp3/cmake_build;-DINCLUDE_CLOUD=ON -DPACKAGE_GENERATOR=DEB -DUPLOAD_BAZEL_REMOTE_CACHE=ON -DINCLUDE_GDB=OFF -DOFFICIAL=OFF -DINCLUDE_DSOPT=OFF;' | grep Ninja".execute()
        // def proc = "echo 'Ninja;/home/tong.zhu/projects/tops_tmp3/tops;/home/tong.zhu/projects/tops_tmp3/cmake_build;-DINCLUDE_CLOUD=ON -DPACKAGE_GENERATOR=DEB -DUPLOAD_BAZEL_REMOTE_CACHE=ON -DINCLUDE_GDB=OFF -DOFFICIAL=OFF -DINCLUDE_DSOPT=OFF;' | awk -F\; '{printf \"Generator:\t%s\nSource Tree:\t%s\nBuild Tree:\t%s\nOptions:\t%s\nCMD:\t\tcmake -S ../tops %s -G %s\n\",\$1,\$2,\$3,\$4,\$4, \$1}'".execute()
        // def proc = 'ls /home'.execute()
        proc.consumeProcessOutput(sout, serr)
        proc.waitForOrKill(1000)
        println "out> $sout\nerr> $serr"
    }

}

