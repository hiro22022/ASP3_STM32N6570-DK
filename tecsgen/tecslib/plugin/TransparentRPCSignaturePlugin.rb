# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 20024-2026 by TOPPERS Project
#--
#   上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
#   ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
#   変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
#   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#       スコード中に含まれていること．
#   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#       の無保証規定を掲載すること．
#   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#       と．
#     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#         作権表示，この利用条件および下記の無保証規定を掲載すること．
#     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#         報告すること．
#   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
#       また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
#       由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
#       免責すること．
#  
#   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
#   よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
#   に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
#   アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
#   の責任を負わない．
#  
#   $Id: TransparentRPCSignaturePlugin.rb 3300 2026-01-04 12:29:45Z okuma-top $
#++

# mikan through plugin: namespace が考慮されていない

require_tecsgen_lib "lib/GenTransparentMarshaler.rb"
require_tecsgen_lib "lib/GenParamCopy.rb"

#==  スループラグインの共通の親クラス　かつ （何もせず）スルーするセルを挿入するスループラグイン
#    スループラグインは ThroughPlugin の子クラスとして定義する
class TransparentRPCSignaturePlugin < SignaturePlugin
TransparentRPCSignaturePlugin
  include GenTransparentMarshaler
  include GenParamCopy
  @@generated_celltype = {}

  # RPCPlugin 専用のオプション
  TransparentRPCPluginArgProc = RPCPluginArgProc.dup  # 複製を作って元を変更しないようにする
  TransparentRPCPluginArgProc[ "noClientSemaphore"  ] = Proc.new { |obj,rhs| obj.set_noClientSemaphore rhs }
  TransparentRPCPluginArgProc[ "semaphoreCelltype"  ] = Proc.new { |obj,rhs| obj.set_semaphoreCelltype rhs }
  TransparentRPCPluginArgProc[ "DataPumpHolder"  ] = Proc.new { |obj,rhs| obj.set_datapumpholder rhs }

  #=== RPCPlugin の initialize
  #  説明は ThroughPlugin (plugin.rb) を参照
  def initialize( signature, option )
    super
    @b_noClientSemaphore = false
    @semaphoreCelltype = "tSemaphore"
    @b_datapumpholder = false
    @call_port_name = :"cCall"

    # オプション：GenTransparentMarshaler 参照
    @plugin_arg_check_proc_tab = TransparentRPCPluginArgProc
    parse_plugin_arg

      # ThroughPlugin では、ｃCall (固定)
      # SignaturePlugin は、デフォルトではシグニチャ名由来の呼び口名が付く
      #    これを ThroughPlugin に合わせて cCall に変更する
    cell_name = "#{signature}Cell"          # dummy
    initialize_transparent_marshaler cell_name

    if @b_datapumpholder == true then
      channel_head_append = "DPH"
    else
      channel_head_append = ""
    end

    @rpc_channel_celltype_name = "tRPCPlugin#{channel_head_append}_#{@TDRCelltype}_#{@signature.get_global_name}"
    @rpc_channel_celltype_name_full = "tRPCPlugin#{channel_head_append}_#{@TDRCelltype}_#{@channelCelltype}_#{@signature.get_global_name}"
    @rpc_channel_celltype_file_name = "#{$gen}/#{@rpc_channel_celltype_name}.cdl"
    # p "TransparentMarhslerPlugin: init: #{@rpc_channel_celltype_file_name}"

    if @signature.need_PPAllocator? then
      if @PPAllocatorSize == nil then
        cdl_error( "RPC9999 PPAllocatorSize must be speicified for oneway [in] array" )
        # @PPAllocatorSize = 0   # 仮に 0 としておく (cdl の構文エラーを避けるため)
      end
    end
  end

  #=== CDL ファイルの生成
  #file::     FILE    生成するファイル
  def gen_cdl_file( file )
    # p "TransparentMarhslerPlugin: gen_cdl_file #{@rpc_channel_celltype_file_name}"
    gen_plugin_decl_code( file )
  end

  #=== plugin の宣言コード (celltype の定義) 生成
  def gen_plugin_decl_code( file )
    file.print <<EOT
/*
 * genratedy by TransparentMarhsalerPlugin
/ */
import( "#{@rpc_channel_celltype_file_name}" );
EOT

    ct_name = @rpc_channel_celltype_file_name
    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ ct_name ] == nil then
      @@generated_celltype[ ct_name ] = [ self ]
    else
      @@generated_celltype[ ct_name ] << self
      return
    end

    gen_marshaler_celltype
    print <<EOT
[TransparentRPCSignaturePlugin]
           create celltype #{@rpc_channel_celltype_name}
                        in #{@rpc_channel_celltype_file_name}
EOT

    if @PPAllocatorSize then
      alloc_cell = "  cell tPPAllocator PPAllocator {\n    heapSize = #{@PPAllocatorSize};\n  };\n"
      alloc_call_port_join = "    cPPAllocator = PPAllocator.ePPAllocator;\n"
    else
      alloc_cell = ""
      alloc_call_port_join = ""
    end

    if @b_noClientSemaphore == false then
      semaphore1 = <<EOT
  /* Semaphore for Multi-task use ("specify noClientSemaphore" option to delete this) / */
  cell #{@semaphoreCelltype} Semaphore {
    initialCount = 1;
    attribute = C_EXP( "TA_NULL" );
  };
EOT
      semaphore2 = "    cLockChannel = Semaphore.eSemaphore;\n"
    else
      semaphore1 = ""
      semaphore2 = ""
    end

    # p "TransparentRPCSignaturePlugin: open #{@rpc_channel_celltype_file_name}"
    f = CFile.open( @rpc_channel_celltype_file_name, "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    f.print <<EOT
/*
 * generated by TransparentRPCSignaturePlugin
 * for signature #{@signature.get_namespace_path}
/ */
import( "#{@marshaler_celltype_file_name}" );
import( <rpc/tDataqueueOWChannel.cdl> );

/****** Client Side Channel / ******/
composite #{@rpc_channel_celltype_name}_ClientSide {
  /* Interface / */
  entry #{@signature.get_namespace_path} eThroughEntry;
  call sTDR       cTDR;
  call sEventflag cEventflag;

  /* Implementation / */
#{semaphore1}
  cell #{@marshaler_celltype_name} #{@signature.get_global_name}_marshaler{
    cTDR         => composite.cTDR;
    cEventflag   => composite.cEventflag;
#{semaphore2}  };
  composite.eThroughEntry => #{@signature.get_global_name}_marshaler.eClientEntry;
};

/****** Server Side Channel / ******/
composite #{@rpc_channel_celltype_name}_ServerSide {
  /* Interface / */
  call #{@signature.get_namespace_path} cServerCall;
  call sTDR       cTDR;
  call sEventflag cEventflag;
  entry sTaskBody eMain;

  /* Implementation / */
#{alloc_cell}  cell #{@unmarshaler_celltype_name} #{@signature.get_global_name}_unmarshaler{
    cTDR        => composite.cTDR;
    cEventflag  => composite.cEventflag;
    cServerCall => composite.cServerCall;
#{alloc_call_port_join}  };
  cell tRPCDedicatedTaskMain RPCTaskMain{
    cMain = #{@signature.get_global_name}_unmarshaler.eUnmarshalAndCallFunction;
  };
  composite.eMain => RPCTaskMain.eMain;
};

/****** Client & Server Combined Channel / ******/
[active]
composite #{@rpc_channel_celltype_name} {
  /* Interface / */
  attr {
    PRI taskPriority;
    size_t  stackSize = 4096;
    ATR     attribute = C_EXP( "TA_ACT" );  /* marshaler starts at the beginning / */
  };
  entry #{@signature.get_namespace_path} eThroughEntry;
  call #{@signature.get_namespace_path} #{@call_port_name};
  call sTDR       cTDR;
  call sEventflag cEventflag;

  /* Implementation / */
  cell #{@rpc_channel_celltype_name}_ClientSide RPCChannel_ClientSide{
    cTDR        => composite.cTDR;
    cEventflag  => composite.cEventflag;
  };
  cell #{@rpc_channel_celltype_name}_ServerSide RPCChannel_ServerSide{
    cTDR        => composite.cTDR;
    cEventflag  => composite.cEventflag;
    cServerCall => composite.#{@call_port_name};
  };
  cell tTask Task {
    cTaskBody = RPCChannel_ServerSide.eMain;
    priority  = composite.taskPriority;
    attribute = composite.attribute;
    stackSize = composite.stackSize;
  };
  composite.eThroughEntry => RPCChannel_ClientSide.eThroughEntry;
};

/****** Client & Server Combined Channel / ******/
[active]
composite #{@rpc_channel_celltype_name_full} {
  /* Interface / */
  attr {
    PRI taskPriority;
    size_t  stackSize = 4096;
    ATR     attribute = C_EXP( "TA_ACT" );  /* marshaler starts at the beginning / */
  };
  entry #{@signature.get_namespace_path} eThroughEntry;
  call #{@signature.get_namespace_path} #{@call_port_name};

  /* Implementation / */
  cell #{@rpc_channel_celltype_name}_ClientSide RPCChannel_ClientSide{
    cTDR         = Channel.eTDR;
    cEventflag   = Channel.eEventflag;
  };
  cell #{@rpc_channel_celltype_name}_ServerSide RPCChannel_ServerSide{
    cTDR         = Channel.eTDR;
    cEventflag   = Channel.eEventflag;
    cServerCall => composite.#{@call_port_name};
  };
  cell tDataqueueOWChannel Channel {
  };
  cell tTask Task {
    cTaskBody = RPCChannel_ServerSide.eMain;
    priority  = composite.taskPriority;
    attribute = composite.attribute;
    stackSize = composite.stackSize;
  };
  composite.eThroughEntry => RPCChannel_ClientSide.eThroughEntry;
};

EOT
    # mikan stackSize option & 最新 tecs_package 対応

    f.close
  end

  #=== プラグイン引数 noClientSemaphore のチェック
  def set_noClientSemaphore rhs
    rhs = rhs.to_sym
    if rhs == :true then
      @b_noClientSemaphore = true
    elsif rhs == :false then
      @b_noClientSemaphore = false
    else
      cdl_error( "RPC9999 specify true or false for noClientSemaphore" )
    end
  end

  #=== プラグイン引数 semaphoreCelltype のチェック
  def set_semaphoreCelltype rhs
    @semaphoreCelltype = rhs.to_sym
    nsp = NamespacePath.analyze( @semaphoreCelltype.to_s )
    obj = Namespace.find( nsp )
    if ! obj.instance_of?( Celltype ) && ! obj.instance_of?( CompositeCelltype ) then
      cdl_error( "RPC9999 semaphoreCelltype '#{rhs}' not celltype or not defined" )
    end
  end
  #=== プラグイン引数 datapumpholder のチェック
  def set_datapumpholder rhs
        rhs = rhs.to_sym
    if rhs == :true then
      @b_datapumpholder = true
    elsif rhs == :false then
      @b_datapumpholder = false
    else
      cdl_error( "RPC9999 specify true or false for datapumpholder" )
    end
  end
end
