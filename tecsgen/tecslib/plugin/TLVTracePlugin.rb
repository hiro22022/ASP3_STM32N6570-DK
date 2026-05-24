# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2014 by TOPPERS Project
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
#   $Id: TracePlugin.rb 1409 2011-05-22 07:21:55Z okuma-top $
#++

# 2014.1.25 修正
# cell の region 対応
# @next_cell_name => @next_cell.get_name  # region が next_cell と同じのため、名前だけでよい
# VoidType 判定を func_type.get_type => func_type.get_original_type

# mikan through plugin: namespace が考慮されていない
class TLVTracePlugin < ThroughPlugin
  
  #
  @@output_celltype_list = {}
  @@output_cell_list = {}
  
  def gen_through_cell_code( file )

    gen_plugin_decl_code( file )

    nest = @region.gen_region_str_pre file
    indent_str =  "  " * nest

    file.print <<EOT
#{indent_str}cell #{@ct_name} #{@cell_name} {
#{indent_str}  #{@call_port_name} = #{@next_cell.get_name}.#{@next_cell_port_name};
#{indent_str}};
EOT
    @region.gen_region_str_post file
  
  puts "------------------------------"
  puts @celltype.get_name
  
  #--TLV用のリソースファイルの出力----------------------
  #呼び元のセルタイプ情報の出力
  generate_celltype_log_tlv @caller_cell.get_celltype
  
  #呼び元のセル情報の出力
  generate_cell_log_tlv @caller_cell

  #呼び先のセルタイプ情報
  generate_celltype_log_tlv @celltype
  #呼び先のセル情報
  @celltype.get_cell_list.each do |cell|
    if @next_cell.get_name.to_s == cell.get_name.to_s
      generate_cell_log_tlv cell
      print "output::" , cell.get_name , "\n"
    else
      print "not output cell::", cell.get_name,"::",@next_cell.get_name, "\n"
    end
  end
  
  
  
  puts "------------------------------"  
  end

  def gen_ep_func_body( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )

    if ! func_type.get_original_type.kind_of?( VoidType ) then
      file.print( "  #{func_type.get_type_str}  retval;\n" )
    end
  
  #時間測定用の変数の宣言
  file.print( "  SYSUTM utime;\n")
  
  if ! b_singleton then

    file.print <<EOT
  #{ct_name}_CB *p_cellcb;
  if( VALID_IDX( idx ) ){
    p_cellcb = #{global_ct_name}_GET_CELLCB(idx);
  }else{
     /* エラー処理コードをここに記述 */
  }

EOT
    end

    p "celltype_name, sig_name, func_name, func_global_name"
    p "#{ct_name}, #{sig_name}, #{func_name}, #{func_global_name}"
    
    #呼出し時間の測定
    file.print( "  getMicroTime(&utime);\n");
    
    #ログ出力開始
    #file.print( "  syslog(LOG_EMERG, \"[%d]#{@next_cell.get_name}.enter(#{func_name}")
    file.print( "  syslog(LOG_INFO, \"time=%dus #{@caller_cell.get_name}->#{@next_cell.get_name}.enter(#{func_name}")
    
    # 呼び元のセルの情報の取得の仕方
    # Cell.get_current.get_name
        
    #パラメタのフォーマット出力:IN
    print_in_params_format( params, file )

    #時間の出力
    file.print(")\", utime")
    #パラーメタの IN
    print_in_params_name(params, file)
    file.print( ");\n" )
    
    #-----関数の呼出し---------------------------------
    #返値の取得
    if ! func_type.get_original_type.kind_of?( VoidType ) then
      file.print( "  retval =" );
    end
    
    file.print( "  #{@call_port_name}_#{func_name}(" )
    
    delim = ""
    params.each{ |param|
      file.printf( "#{delim} #{param.get_name}" )
      delim = ","
    }
    file.print( ");\n" )
    #--------------------------------------------------    
    
    #時間の測定
    file.print( "  getMicroTime(&utime);\n");

    #file.print( "  syslog(LOG_EMERG, \"[%d]#{@next_cell.get_name}.leave(#{func_name}")
    file.print( "  syslog(LOG_INFO, \"time=%dus #{@caller_cell.get_name}->#{@next_cell.get_name}.leave(#{func_name}")
    
    #返値のformat
    if ! func_type.get_original_type.kind_of?( VoidType ) then
      file.print(", retval=%d");
    end
    
    #パラーメタのフォーマットの出力:OUT
    print_out_params_format(params, file)
    
    #時間の出力
    file.print(")\", utime")
    
    #返値の出力
    if ! func_type.get_original_type.kind_of?( VoidType ) then
      file.print( ", retval" );
    end

    #パラーメタの OUT
    print_out_params_name(params, file)
    
=begin    
    print_params( params, file, 0, :OUT )
    if( ! func_type.get_original_type.kind_of?( VoidType ) ) then
      print_param( "retval", func_type.get_type, file, 0, :RETURN, nil, nil)
      file.print( "  return retval;\n" )
    end
=end
    file.print( " );\n") 
  end
  
  
  # IN:パラメータのフォーマットの出力
  def print_in_params_format(params, file)
    params.each { |param|
      dir = param.get_direction
      #入力のみ扱う
      next if dir == :OUT || dir == :RECEIVE
      
      #パラメータフォーマットの出力
      print_param_format(param, file)
    }
  end
  
  # IN:パラメータの名出力
  def print_in_params_name(params, file)
    params.each { |param|
      dir = param.get_direction
      #入力のみ扱う
      next if dir == :OUT || dir == :RECEIVE
      
      #パラメータ名の出力
      print_param_name(param, file)
    }
  end
  
  def print_out_params_format(params, file)
    params.each { |param|
      dir = param.get_direction
      #出力のみ扱う
      next if dir == :IN || dir == :SEND
      
      #パラメータフォーマットの出力
      print_param_format(param, file)
    }
  end

  # OUT:パラメータの名出力
  def print_out_params_name(params, file)
    params.each { |param|
      dir = param.get_direction
      #出力のみ扱う
      next if dir == :IN || dir == :SEND
      
      #パラメータ名の出力
      print_param_name(param, file)
    }
  end
  
  #パラメータフォーマットの出力
  def print_param_format(param, file)
    
    file.print ", ", param.get_name , "="
    
    #TODO typeごとに変更
    case param.get_type
    when PtrType
      if param.get_type.get_type_str == "char_t*" || param.get_type.get_type_str == "const char_t*"
        file.print "%d"
      else
        file.print "%d"
      end  
    when ArrayType, FuncType, PtrType, IntType, StructType, VoidType, FloatType, DefinedType, BoolType
      file.print "%d"
    end
  end
  
  # パラメータの名出力
  # 
  def print_param_name(param, file)
    
    file.print ", "
    
    #TODO typeごとに変更する
    case param.get_type
    when FuncType, IntType, StructType, VoidType, FloatType, DefinedType, BoolType
      file.print  param.get_name
    when ArrayType, PtrType
      file.print  "*", param.get_name
    end
  end
  
=begin
  def print_params( params, file, nest, direction )
    params.each{ |param|
      dir = param.get_direction
      if( direction == :IN )then
        case dir
        when :IN, :INOUT, :SEND
          print_param( param.get_name, param.get_type, file, nest, dir, nil, nil )
        end
      else
        case dir
        when :OUT, :INOUT, :RECEIVE
          print_param( param.get_name, param.get_type, file, nest, dir, nil, nil )
        end
      end
    }
  end

  def print_param( name, type, file, nest, direction, outer, outer2 )
    #indent = "  " * ( nest + 1 )
    indent = " "
    case type
    when DefinedType
      print_param( name, type.get_type, file, nest, direction, outer, outer2 )
    when VoidType
    when BoolType
      file.print( ",#{indent}[#{direction}]#{type.get_type_str} #{outer}#{name}#{outer2} = %d;\\n\", #{outer}#{name}#{outer2}" )
    when IntType
      file.print( ",#{indent}[#{direction}]#{type.get_type_str} #{outer}#{name}#{outer2} = %ld;\\n\", (long)#{outer}#{name}#{outer2}" )
    when FloatType
      file.print( ",#{indent}[#{direction}]#{type.get_type_str} #{outer}#{name}#{outer2} = %g;\\n\", (double)#{outer}#{name}#{outer2}" )
    when EnumType  # mikan EnumType

    when StructType
      members_decl =type.get_members_decl
      members_decl.get_items.each { |m|
        print_param( m.get_name, m.get_type, file, nest, direction, "#{outer}#{name}.", nil )
      }
    when FuncType  # mikan FuncType
    when ArrayType # mikan ArrayType
    when PtrType

      se = type.get_size
      ce = type.get_count
      max_loop = 8

      if se then
        loop_count = "(((#{se.to_s})>#{max_loop}) ? #{max_loop} : (#{se.to_s}))"
        file.print( "#{indent}size_is(#{se.to_s})=%d\\n\", #{se.to_s}" )
      elsif ce then
        loop_count = "(((#{ce.to_s})>#{max_loop}) ? #{max_loop} : (#{ce.to_s})) "
        file.print( "#{indent}count_is(#{ce.to_s})=%d\\n\", #{ce.to_s}" )
      else
        loop_count = nil
      end

      # mikan PtrType: string

      referto = type.get_referto
      type = referto
      if type.kind_of?( DefinedType ) then
        type = type.get_original_type
      end

      if loop_count then
        file.print <<EOT
  {
     int i__, loop_count__ = #{loop_count};
     for( i__ = 0; i__ < loop_count__; i__++ ){
EOT
      end

      case type
      when StructType
        members = type.get_members_decl
        members.get_items.each { |m|
          if loop_count then
            outer = "#{name}[i__]."
          else
            outer = "#{name}->"
          end
          print_param( m.get_name, m.get_type, file, nest, direction, outer, nil )
        }
      when FuncType  # mikan FuncType
      when ArrayType # mikan ArrayType
      when BoolType, IntType, FloatType, EnumType
        if loop_count then
          outer = nil
          outer2 = "[i__]"
        else
          outer = "*"
          outer2 = nil
        end
        print_param( name, type, file, nest, direction, outer, outer2 )
      when PtrType
        type = type.get_referto
        if loop_count then
          outer = "(*"
          outer2 = ")[i__]"
        else
          outer = "**"
          outer2 = nil
        end
        print_param( name, type, file, nest, direction, outer, outer2 )
      end

      if loop_count then
        file.print <<EOT

     } /* for ( i__ ) */
  }
EOT
      end
    end
  end
=end
  

  def gen_factory( file )
    file.print ""
    file.print <<EOT
    FACTORY {
 write("$ct$_factory.#{$h_suffix}", "#include $ct$\\"syslog.h\\"");      
//      write("test_factory.h", "#include \\"syslog.h\\"");
    };
EOT
    file.print ""

end


  def generate_log_tlv
    generate_celltype_log_tlv self
    generate_cell_log_tlv
  end

  # TLV用のリソースタイプ(セルタイプの生成
  # takuya 2009/01/12
  def generate_celltype_log_tlv( celltype )
    
    #既に出力済み
    if(@@output_celltype_list[ celltype ])
      return
    else
      @@output_celltype_list[ celltype ] = celltype
    end
        
    # 出力ファイルを開く
    f = AppFile.open( "#{$gen}/tecs.resh")
    #f = AppFile.open( "#{$gen}/#{celltype.get_name}.resh")

    tab = "\t" * 2
    #f.print("\"")
    # セルタイプを出力
    #f.print "\t" * 2 + "\"" + @global_name.to_s + "\":{\n"
    f.print "\t" * 2 + "\"" + celltype.get_name.to_s + "\":{\n"
    # 表示文字の生成
    f.print "\t" * 3 + "\"DisplayName\": \""+ celltype.get_name.to_s + "\",\n"

    # 呼び口の出力
    celltype.get_port_list.each do |call|
      if call.get_port_type == :CALL then
        f.print "\t" * 4 + "\""+ call.get_name.to_s+ "\":{\n"
        generate_log_tlv_signature( call.get_signature, 4, f)
        f.print "\t" * 4 + "},\n"
      end
    end

    # 受け口の出力
    celltype.get_port_list.each do |entry|
      if entry.get_port_type != :CALL then
        f.print "\t" * 4 + "\""+ entry.get_name.to_s+ "\":{\n"

        f.print "\t" * 4 + "},\n"
      end
    end
    # Attriuteの出力
    f.print "\t" * 3 + "\"Attributes\":{\n" if celltype.get_attribute_list.size != 0

    # 属性の出力
    celltype.get_attribute_list.each do |att|
      f.print "\t" * 4 + "\""+ att.get_name.to_s+ "\":{\n"

      f.print "\t" * 4 + "},\n"
    end

    # Attriuteの出力の終了
    f.print "\t" * 3 + "}\n" if celltype.get_attribute_list.size !=0

    # セルタイプの出力の終了
    f.print "\t" * 2 + "}\n"
    f.close
  end

  # TLV用のリソースタイプ（シグニチャ情報）の生成
  # takuya 2009/01/12
  def generate_log_tlv_signature( signature, indent, f)
    # シグニチャ名の出力
    f.print "\t" * (indent + 1) + "\""+ signature.get_name.to_s+ "\":{\n"
    #関数の出力
    signature.get_function_head_array.each do |func|
      f.puts "\t" * (indent + 2) + func.get_name.to_s
    end
    f.print "\t" * (indent + 1) + "},\n"
  end
  
  # TLV用のリソースタイプ（セル情報）の生成
  # takuya 2009/01/19
  def generate_cell_log_tlv( cell )
    #既に出力済みかどうか
    if(@@output_cell_list[ cell ])
      return
    else
      @@output_cell_list[ cell ] = cell
    end
    
    # 出力ファイルを開く
    f = AppFile.open( "#{$gen}/tecs.res")
    #f = AppFile.open( "#{$gen}/#{@name}.res")
    #get_cell_list.each do |cell|
    #f.print cell.get_name , "::" ,@global_name.to_s + "\":{\n"
    f.puts cell.get_name
    f.close
  end
end
