-------------------------------------------------------------------------------------
README
                                                                           2016/08/31
-------------------------------------------------------------------------------------
■はじめに
  本テキストは、McKernel向けテストセットの使用手引きについて記載する。


-------------------------------------------------------------------------------------
■ディレクトリ構造
  <root>
    `- ostest/
        |- README.txt                               :本テキストファイル
        |- NG_item_list.txt                         :NG項目詳細テキストファイル
        |
        |- install/                                 :IHK/McKernelインストール先ディレクトリ
        |
        |- list/
        |   `- test_list.xlsx                       :テスト項目リスト
        |
        |- log/
        |   `- run_test_x86_log.txt                 :run_test_x86.sh 実行ログ
        |
        |- src/
        |   |- Makefile                             :テストセットビルド用Makefile
        |   |- coredump_util/                       :coredumpテスト向けGDB構成情報
        |   |- execve_app/                          :execve向けテストバイナリソース
        |   |- hello_world/                         :hello_world出力テストソース
        |   |- large_bss/                           :bss領域を大きく確保するテストソース
        |   |- lv07_read-write_with-glibc/          :動的ライブラリリンクテストソース
        |   |- lv09_syscall_page_fault_forwqarding/ :ページフォルトフォワーディングテストソース
        |   |- lv11_copy_on_write/                  :コピーオンライトテストソース
        |   |- lv12_signal/                         :シグナルテストソース
        |   |- lv14_large_pages/                    :ラージページテストソース
        |   |- lv15_continuous_execution/           :連続実行テストソース
        |   |- shellscript/                         :シェルスクリプトテスト用スクリプト
        |   |- show_affinity/                       :sched_getaffinityテスト用サポートバイナリソース
        |   |- socket_TP/                           :ソケットテストソース
        |   |- test_drv/                            :テストドライバソース
        |   `- test_mck/                            :テストプログラムソース
        |
        `- util/                                    :テスト実行スクリプト
            |- insmod_test_drv.sh                   :テストドライバinsmodスクリプト
            |- rmmod_test_drv.sh                    :テストドライバrmmodスクリプト
            |- run_test_x86.sh                      :テストセット実行スクリプト(x86)
            `- run_testset_x86.sh                   :テストセットリスト(x86)


-------------------------------------------------------------------------------------
■TPビルド方法
  ※TPのビルドはドライバを含むため、McKernel実行環境上で行う必要がある。
    ビルド済みバイナリは同梱していない。

  $ cd <root>/ostest/src/
  $ make

  上記makeによって、<root>/ostest/bin配下にTPバイナリが生成される。


-------------------------------------------------------------------------------------
■TP実行方法
  ※各テスト実行用スクリプトは、<root>/ostest/installにIHK/McKernelが
    インストールされている前提で作成している。
    そのため、IHK/McKernelを<root>/ostest/installにインストールする必要がある。

  $ cd <src_dir>/ihk
  $ ./configure --with-target=smp-x86 --prefix=<root>/ostest/install
  $ cd <src_dir>/mckernel
  $ ./configure --with-target=smp-x86 --prefix=<root>/ostest/install
  $ cd <src_dir>/ihk
  $ make && make install
  $ cd <src_dir>/mckernel
  $ make && make install

  NG項目を除くテストセットの実行は、以下コマンドを実行する。

  $ cd <root>/ostest
  $ sh util/run_test_x86.sh -n

  run_test_x86.shのオプションは以下の通り。
    -nオプション：通常実行
    -eオプション：execve経由実行
    -bオプション：内部で-n実行後に-eを実行
    -Nオプション：NG項目を含んだテストセットを実行
    -Hオプション：ホストLinux上でテストセットを実行
    -hオプション：usageを表示

  run_test_x86.shの内部でmcreboot.shおよびmcstop+release.shを実行するため、
  事前にMcKernelを起動しておく必要はない。
  mcreboot.shは下記条件で実行する。
    - CPUコア数：コア番号0以外のコアをMcKernelに割り当てる
    - メモリ量 ：空きメモリ量のうちの45％をMcKernelに割り当てる

  mcreboot.shの外部からCPUコア数、メモリ量を明示的に指定する形を取っているため、
  下記版数以降のmcreboot.shである必要がある。

  commit e12997e6a906de509866545623b5d3cae6c1720d
  Author: Balazs Gerofi <bgerofi@riken.jp>
  Date:   Tue Jun 21 08:49:33 2016 -0700

      mcreboot: support for CPU cores (-c) and memory (-m) arguments


-------------------------------------------------------------------------------------
■TP結果確認方法
  TP結果の確認は、下記のログファイルとの比較で行う。

  <root>/ostest/log/run_test_x86_log.txt

  差分については、実行毎／環境で変化する可能性がある項目も含まれるため、
  各差分について実行毎／環境で変化する差分であるかどうかを評価する。

  主な実行毎／環境で変化するものは以下の通り。
  ・プロセスID
  ・メモリアドレス
  ・マルチスレッドTPのログ出力順序
  ・一時ファイルのファイル名
  ・μ秒単位のタイマ値


-------------------------------------------------------------------------------------
■備考
  - 同梱のテストセットリストスクリプトは、NG_item_list.txtでNGとなっている項目については
    実行時引数で-Nを付与しないと実行しないようにしている。

  - run_test_x86_log.txtの実行ログについても、NG項目を除外した
    テストセットリストスクリプトを実行した結果のログである。

  - run_test_x86.shの-e、-bの結果についてはexecveがNGのため、実行ログには含めていない。


以上。

-------------------------------------------------------------------------------------
                                                       COPYRIGHT FUJITSU LIMITED 2016
