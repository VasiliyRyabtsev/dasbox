//
// AUTO-GENERATED FILE - DO NOT EDIT!!
//

"options indenting = 4\n"
"options no_unused_block_arguments = false\n"
"options no_unused_function_arguments = false\n"
"options no_aot = true\n"
"\n"
"module ast_boost shared public\n"
"\n"
"require rtti\n"
"require ast public\n"
"require strings\n"
"require daslib/strings_boost\n"
"\n"
"def isVectorType ( typ:Type )\n"
"    return (\n"
"        typ==Type tInt2   || typ==Type tInt3   || typ==Type tInt4   ||\n"
"        typ==Type tUInt2  || typ==Type tUInt3  || typ==Type tUInt4  ||\n"
"        typ==Type tFloat2 || typ==Type tFloat3 || typ==Type tFloat4 ||\n"
"        typ==Type tRange  || typ==Type tURange\n"
"    )\n"
"\n"
"def describe(list:AnnotationArgumentList)\n"
"    return join([{ for arg in list; \"{arg.name}={get_annotation_argument_value(a"
"rg)}\" }],\",\")\n"
"\n"
"def describe(ann:AnnotationDeclaration)\n"
"    if ann.arguments.length != 0\n"
"        return \"{ann.annotation.name}({describe(ann.arguments)})\"\n"
"    else\n"
"        return \"{ann.annotation.name}\"\n"
"\n"
"def describe(list:AnnotationList)\n"
"    return join([{ for arg in list; describe(*arg) }], \",\")\n"
"\n"
"def describe_bitfield(bf;merger:string = \"\")\n"
"    static_if typeinfo(is_bitfield bf)\n"
"        let bftd <- typeinfo(ast_typedecl bf)\n"
"        return join( [{ for name,bit in bftd.argNames,range(32); \"[{name}]\";\n"
"            where (int(bf) & (1<<bit)) != 0 }], merger )\n"
"    else\n"
"        concept_assert(false,\"requires bitfield\")\n"
"\n"
"def describe_function_short(func:FunctionPtr)\n"
"    var args <- [{for x in func.arguments; \"{x.name}:{describe(x._type,false)}\" "
"}]\n"
"    var res = \"{func.name} ({join(args,\";\")}) : {describe(func.result,false)}\"\n"
"    delete args\n"
"    return res\n"
"\n"
"def isExpression(t:TypeDeclPtr; top:bool=true) : bool\n"
"    if t==null\n"
"        return false\n"
"    elif t.dim.length!=0\n"
"        return false\n"
"    elif t.baseType==Type tHandle\n"
"        if eq(t.annotation._module.name,\"ast\")\n"
"            return starts_with(\"{t.annotation.name}\",\"Expr\")\n"
"        else\n"
"            return false\n"
"    elif t.baseType==Type tPointer && top\n"
"        return isExpression(t.firstType,false)\n"
"    else\n"
"        return false\n"
"\n"
"def is_same_or_inherited ( parent,child: Structure? )\n"
"    var ch = child\n"
"    while ch != null\n"
"        if parent == ch\n"
"            return true\n"
"        ch = ch.parent\n"
"    return false\n"
"\n"
"def is_class_method ( cinfo:StructurePtr; finfo:TypeDeclPtr )\n"
"    if finfo.baseType != Type tFunction\n"
"        return false\n"
"    if finfo.dim.length !=0\n"
"        return false\n"
"    if finfo.argTypes.length == 0\n"
"        return false\n"
"    if finfo.argTypes[0].baseType != Type tStructure\n"
"        return false\n"
"    if finfo.argTypes[0].dim.length != 0\n"
"        return false\n"
"    if !is_same_or_inherited(finfo.argTypes[0].structType,get_ptr(cinfo))\n"
"        return false\n"
"    return true\n"
"\n"
"def emplace_new ( var vec:dasvector`smart_ptr`Expression; var ptr:smart_ptr<Expr"
"ession> )\n"
"    emplace(vec,ptr)\n"
"\n"
"def emplace_new ( var vec:dasvector`smart_ptr`TypeDecl; var ptr:smart_ptr<TypeDe"
"cl> )\n"
"    emplace(vec,ptr)\n"
"\n"
"def emplace_new ( var vec:dasvector`smart_ptr`Variable; var ptr:smart_ptr<Variab"
"le> )\n"
"    emplace(vec,ptr)\n"
"\n"
"def emplace_new ( var vec:MakeStruct; var ptr:smart_ptr<MakeFieldDecl> )\n"
"    emplace(vec,ptr)\n"
"\n"
"def override_method ( var str:StructurePtr; name,funcName:string ) : bool\n"
"    for fld in str.fields\n"
"        if eq(fld.name,name)\n"
"            var vcast <- new [[ExprCast() at=str.at,\n"
"                subexpr <- new [[ExprAddr() at=str.at,target:=funcName]],\n"
"                castType <- new [[TypeDecl() at=str.at,baseType=Type autoinfer]]"
"\n"
"            ]]\n"
"            fld.init := null\n"
"            fld.init <- vcast\n"
"            return true\n"
"    return false\n"
"\n"
"class MacroMacro : AstFunctionAnnotation\n"
"    def override apply ( var func:FunctionPtr; var group:ModuleGroup; args:Annot"
"ationArgumentList; var errors : das_string ) : bool\n"
"        compiling_program().flags |= ProgramFlags needMacroModule\n"
"        func.flags |= FunctionFlags macroInit\n"
"        var blk <- new [[ExprBlock() at=func.at]]\n"
"        var ifm <- new [[ExprCall() at=func.at, name:=\"is_compiling_macros\"]]\n"
"        var ife <- new [[ExprIfThenElse() at=func.at, cond<-ifm, if_true<-func.b"
"ody]]\n"
"        emplace(blk.list,ife)\n"
"        func.body <- blk\n"
"        return true\n"
"\n"
"class TagFunctionAnnotation : AstFunctionAnnotation\n"
"    def override apply ( var func:FunctionPtr; var group:ModuleGroup; args:Annot"
"ationArgumentList; var errors : das_string ) : bool\n"
"        for arg in args\n"
"            let val = get_annotation_argument_value(arg)\n"
"            if !(val is tBool)\n"
"                errors := \"tag_function annotation expects only tag names\"\n"
"                return false\n"
"        func.flags |= FunctionFlags exports\n"
"        return true\n"
"\n"
"class TagStructureAnnotation : AstStructureAnnotation\n"
"    def override apply ( var str:StructurePtr; var group:ModuleGroup; args:Annot"
"ationArgumentList; var errors : das_string ) : bool\n"
"        for arg in args\n"
"            let val = get_annotation_argument_value(arg)\n"
"            if !(val is tBool)\n"
"                errors := \"tag_structure annotation expects only tag names\"\n"
"                return false\n"
"        return true\n"
"\n"
"def private for_each_tag_function(mod:Module?; tag:string; blk : block<(var func"
":FunctionPtr):void> )\n"
"    for_each_function(mod, \"\") <| $ ( func )\n"
"        for ann in func.annotations\n"
"            if ann!=null && eq(ann.annotation.name,\"tag_function\")\n"
"                for t in ann.arguments\n"
"                    if eq(t.name,tag)\n"
"                        invoke(blk, func)\n"
"    for_each_generic(mod, \"\") <| $ ( func )\n"
"        for ann in func.annotations\n"
"            if ann!=null && eq(ann.annotation.name,\"tag_function\")\n"
"                for t in ann.arguments\n"
"                    if eq(t.name,tag)\n"
"                        invoke(blk, func)\n"
"\n"
"def find_arg(argn:string; args:AnnotationArgumentList) : RttiValue\n"
"    for a in args\n"
"        if eq(a.name,argn)\n"
"            return get_annotation_argument_value(a)\n"
"    return RttiValue_nothing()\n"
"\n"
"def private apply_tag_annotation(tag:string; ann:FunctionAnnotationPtr)\n"
"    for_each_tag_function(this_module(), tag) <| $ ( func )\n"
"        var tagAnn := ann\n"
"        add_function_annotation(func, tagAnn)\n"
"\n"
"def setup_tag_annotation(name,tag:string; classPtr)\n"
"    var ann <- make_function_annotation(name, classPtr)\n"
"    apply_tag_annotation(tag, ann)\n"
"    this_module() |> add_function_annotation(ann)\n"
"\n"
"def find_unique_function ( mod : Module?; name : string; canfail:bool = false )\n"
"    var res : FunctionPtr\n"
"    var count = 0\n"
"    for_each_function(mod, name) <| $ ( func )\n"
"        res <- func\n"
"        count ++\n"
"    if count > 1\n"
"        if !canfail\n"
"            panic(\"more than one function encountered\")\n"
"        return [[FunctionPtr]]\n"
"    return res\n"
"\n"
"def find_unique_generic ( mod : Module?; name : string; canfail:bool = false )\n"
"    var res : FunctionPtr\n"
"    var count = 0\n"
"    for_each_generic(mod, name) <| $ ( func )\n"
"        res <- func\n"
"        count ++\n"
"    if count > 1\n"
"        if !canfail\n"
"            panic(\"more than one generic function encountered\")\n"
"        return [[FunctionPtr]]\n"
"    return res\n"
"\n"
"def setup_macro(name: string; at:LineInfo) : ExprBlock?\n"
"    compiling_program().flags |= ProgramFlags needMacroModule\n"
"    var fn <- compiling_module() |> find_unique_function(name)\n"
"    if fn == null\n"
"        fn <- new [[Function() at=at, atDecl=at, name:=\"{name}\"]]\n"
"        fn.flags |= FunctionFlags generated\n"
"        fn.flags |= FunctionFlags macroInit\n"
"        fn.result <- new [[TypeDecl() baseType=Type autoinfer, at=at]]\n"
"        var blk <- new [[ExprBlock() at=at]]\n"
"        var iblk <- new [[ExprBlock() at=at]]\n"
"        let res = get_ptr(iblk)\n"
"        var ifm <- new [[ExprCall() at=at, name:=\"is_compiling_macros_in_module\""
"]]\n"
"        var ifmn <- new [[ExprConstString() at=at, value:=compiling_module().nam"
"e]]\n"
"        emplace(ifm.arguments,ifmn)\n"
"        var ife <- new [[ExprIfThenElse() at=at, cond<-ifm, if_true<-iblk]]\n"
"        emplace(blk.list,ife)\n"
"        fn.body <- blk\n"
"        if !(compiling_module() |> add_function(fn))\n"
"            panic(\"failed to setup macro, can't add function {fn.name}\")\n"
"        return res\n"
"    else\n"
"        if fn.body.__rtti != \"ExprBlock\"\n"
"            panic(\"expecting func.ExprBlock\")\n"
"        unsafe\n"
"            var blk = reinterpret<ExprBlock?> fn.body\n"
"            if blk.list.length!=1 || blk.list[0].__rtti!=\"ExprIfThenElse\"\n"
"                panic(\"expecting is_compiling_macros\")\n"
"            var ife = reinterpret<ExprIfThenElse?> blk.list[0]\n"
"            if ife.if_true.__rtti != \"ExprBlock\"\n"
"                panic(\"expecting ife.if_true.ExprBlock\")\n"
"            return reinterpret<ExprBlock?> ife.if_true\n"
"\n"
"class SetupAnyAnnotation : AstStructureAnnotation\n"
"    annotation_function_call : string = \"\"\n"
"    name : string\n"
"    def setup_call(var st:StructurePtr;var cll:smart_ptr<ExprCall>)\n"
"        emplace_new(cll.arguments,new [[ExprConstString() at=st.at, value:=name]"
"])\n"
"    def override apply ( var st:StructurePtr; var group:ModuleGroup; args:Annota"
"tionArgumentList; var errors : das_string ) : bool\n"
"        let argName = find_arg(\"name\", args)\n"
"        if argName is nothing\n"
"            name = \"{st.name}\"\n"
"        elif !(argName is tString)\n"
"            errors := \"name must be string\"\n"
"            return false\n"
"        else\n"
"            name = argName as tString\n"
"        var blk <- setup_macro(\"__setup_macros\", st.at)\n"
"        var cll <- new [[ExprCall() at=st.at, name:=annotation_function_call]]\n"
"        self->setup_call(st,cll)\n"
"        var nee <- new [[ExprNew() at=st.at, initializer=true]]\n"
"        nee.typeexpr <- new [[TypeDecl() baseType=Type tStructure, structType=ge"
"t_ptr(st), at=st.at]]\n"
"        emplace(cll.arguments,nee)\n"
"        emplace(blk.list,cll)\n"
"        return true\n"
"\n"
"class SetupFunctionAnnotatoin : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_function_annotation\"\n"
"\n"
"class SetupBlockAnnotatoin : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_block_annotation\"\n"
"\n"
"class SetupStructureAnnotatoin : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_structure_annotation\"\n"
"\n"
"class SetupEnumerationAnnotation : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_enumeration_annotation"
"\"\n"
"\n"
"class SetupContractAnnotatoin : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_contract_annotation\"\n"
"\n"
"class SetupReaderMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_reader_macro\"\n"
"\n"
"class SetupVariantMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_variant_macro\"\n"
"\n"
"class SetupCallMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_call_macro\"\n"
"\n"
"class SetupTypeInfoMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_typeinfo_macro\"\n"
"\n"
"class SetupInferMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_infer_macro\"\n"
"\n"
"class SetupDirtyInferMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_dirty_infer_macro\"\n"
"\n"
"class SetupLintMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_lint_macro\"\n"
"\n"
"class SetupGlobalLintMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_global_lint_macro\"\n"
"\n"
"class SetupOptimizationMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"add_new_optimization_macro\"\n"
"\n"
"class TagFunctionMacro : SetupAnyAnnotation\n"
"    override annotation_function_call : string = \"setup_tag_annotation\"\n"
"    tag : string\n"
"    def override setup_call(var st:StructurePtr;var cll:smart_ptr<ExprCall>)\n"
"        emplace_new(cll.arguments,new [[ExprConstString() at=st.at, value:=name]"
"])\n"
"        emplace_new(cll.arguments,new [[ExprConstString() at=st.at, value:=tag]]"
")\n"
"    def override apply ( var st:StructurePtr; var group:ModuleGroup; args:Annota"
"tionArgumentList; var errors : das_string ) : bool\n"
"        let argTag = find_arg(\"tag\", args)\n"
"        if argTag is nothing\n"
"            errors := \"missing tag argument\"\n"
"            return false\n"
"        elif !(argTag is tString)\n"
"            errors := \"tag must be string\"\n"
"            return false\n"
"        tag = argTag as tString\n"
"        return SetupAnyAnnotation`apply(cast<SetupAnyAnnotation> self,st,group,a"
"rgs,errors)\n"
"\n"
"def public panic_expr_as : void?\n"
"    panic(\"invalid 'as' expression or null pointer dereference\")\n"
"    return null\n"
"\n"
"def private makeAsOrSafeAs ( expr; var enull:ExpressionPtr )\n"
"    var vdr <- new [[ExprField() at=expr.at, name:=\"__rtti\", value <- clone_expr"
"ession(expr.value)]]\n"
"    var cna <- new [[ExprConstString() at=expr.at, value:=expr.name]]\n"
"    var cond <- new [[ExprOp2() at=expr.at, op:=\"==\", left<-vdr, right<-cna]]\n"
"    var ctype <- new [[TypeDecl() at=expr.at,\n"
"        baseType=Type tPointer,\n"
"        firstType <- new [[TypeDecl() at=expr.at,baseType=Type alias,alias:=expr"
".name]]\n"
"    ]]\n"
"    var esub <- clone_expression(expr.value)\n"
"    if expr.value._type.baseType==Type tPointer\n"
"        var neqz <- new [[ExprOp2() at=expr.at, op:=\"!=\",\n"
"            left <-clone_expression(expr.value),\n"
"            right <-new [[ExprConstPtr() at=expr.at]]\n"
"        ]]\n"
"        var nand <- new [[ExprOp2() at=expr.at, op:=\"&&\",\n"
"            left <- neqz,\n"
"            right <- cond\n"
"        ]]\n"
"        cond <- nand\n"
"    else\n"
"        var nsub <- new [[ExprRef2Ptr() at=expr.at,\n"
"            subexpr<-esub,\n"
"            genFlags = ExprGenFlags alwaysSafe\n"
"        ]]\n"
"        esub <- nsub\n"
"    var vcast <- new [[ExprCast() at=expr.at,\n"
"        subexpr <- esub,\n"
"        castType <- ctype,\n"
"        castFlags = ExprCastFlags reinterpretCast,\n"
"        genFlags = ExprGenFlags alwaysSafe\n"
"    ]]\n"
"    var tri <- new [[ExprOp3() at=expr.at, op:=\"?\",\n"
"        subexpr <- cond,\n"
"        left <- vcast,\n"
"        right <- enull\n"
"    ]]\n"
"    return tri\n"
"\n"
"// replacing ExprIsVariant(value,name) => ExprOp2('==\",value.__rtti,\"name\")\n"
"// if value is ast::Expr*\n"
"class BetterRttiVisitor : AstVariantMacro\n"
"    def override visitExprIsVariant ( prog:ProgramPtr; mod:Module?;expr:smart_pt"
"r<ExprIsVariant> ) : ExpressionPtr\n"
"        if isExpression(expr.value._type)\n"
"            var vdr <- new [[ExprField() at=expr.at, name:=\"__rtti\", value <- cl"
"one_expression(expr.value)]]\n"
"            var cna <- new [[ExprConstString() at=expr.at, value:=expr.name]]\n"
"            var veq <- new [[ExprOp2() at=expr.at, op:=\"==\", left<-vdr, right<-c"
"na]]\n"
"            return veq\n"
"        return [[ExpressionPtr]]\n"
"    def override visitExprAsVariant ( prog:ProgramPtr; mod:Module?; expr:smart_p"
"tr<ExprAsVariant> ) : ExpressionPtr\n"
"        if isExpression(expr.value._type)\n"
"            return makeAsOrSafeAs(expr, new [[ExprCall() at=expr.at, name:=\"pani"
"c_expr_as\"]])\n"
"        return [[ExpressionPtr]]\n"
"    def override visitExprSafeAsVariant ( prog:ProgramPtr; mod:Module?; expr:sma"
"rt_ptr<ExprSafeAsVariant> ) : ExpressionPtr\n"
"        if isExpression(expr.value._type)\n"
"            return makeAsOrSafeAs(expr, new [[ExprConstPtr() at=expr.at]])\n"
"        return [[ExpressionPtr]]\n"
"\n"
"def private walk_and_convert_array ( data : uint8?; info:TypeDeclPtr; at:LineInf"
"o ) : ExpressionPtr\n"
"    let total = any_array_size(data)\n"
"    if total != 0\n"
"        let stride = info.firstType.sizeOf\n"
"        var mkArr <- new [[ExprMakeArray() at=at, makeType<-clone_type(info.firs"
"tType)]]\n"
"        any_array_foreach(data, stride) <| $ ( value )\n"
"            var elem <- walk_and_convert(value, info.firstType, at)\n"
"            emplace(mkArr.values, elem)\n"
"        push(mkArr.makeType.dim,total)\n"
"        var mkToArrayMove <- new [[ExprCall() at=at, name:=\"to_array_move\"]]\n"
"        emplace(mkToArrayMove.arguments,mkArr)\n"
"        return mkToArrayMove\n"
"    else\n"
"        var mkArr <- new [[ExprMakeStruct() at=at, makeType<-clone_type(info)]]\n"
"        return mkArr\n"
"\n"
"def private walk_and_convert_dim ( data : uint8?; info:TypeDeclPtr; at:LineInfo "
") : ExpressionPtr\n"
"	let stride = info.baseSizeOf\n"
"	let total = info.countOf\n"
"	var einfo <- clone_type(info)\n"
"	clear(einfo.dim)\n"
"	var mkArr <- new [[ExprMakeArray() at=at, makeType := info]]\n"
"	for x in range(total)\n"
"        unsafe\n"
"            var elem <- walk_and_convert(data + stride*x, einfo, at)\n"
"            emplace(mkArr.values, elem)\n"
"    einfo := null\n"
"	return mkArr\n"
"\n"
"def private walk_and_convert_pointer ( data : uint8?; info:TypeDeclPtr; at:LineI"
"nfo ) : ExpressionPtr\n"
"    unsafe\n"
"    	let pdata = *(reinterpret<uint8??> data)\n"
"        var elem <- walk_and_convert(pdata, info.firstType, at)\n"
"        var mkAsc <- new [[ExprAscend() at=at, subexpr <- elem]]\n"
"        return mkAsc\n"
"\n"
"def private walk_and_convert_tuple ( data : uint8?; info:TypeDeclPtr; at:LineInf"
"o ) : ExpressionPtr\n"
"    var mkArr <- new [[ExprMakeTuple() at=at, recordType<-clone_type(info)]]\n"
"    for idx in range( info.argTypes.length)\n"
"        let offset = get_tuple_field_offset(info,idx)\n"
"        unsafe\n"
"            var elem <- walk_and_convert(data + offset, info.argTypes[idx], at)\n"
"            emplace(mkArr.values, elem)\n"
"    return mkArr\n"
"\n"
"def private walk_and_convert_structure ( data : uint8?; info:TypeDeclPtr; at:Lin"
"eInfo ) : ExpressionPtr\n"
"	var mkStruct <- new [[ExprMakeStruct() at=at, makeType<-clone_type(info)]]\n"
"	let stype = info.structType\n"
"    var mkS <- new [[ MakeStruct() ]]\n"
"	for x in stype.fields\n"
"        if find_arg(\"do_not_convert\",x.annotation) is nothing\n"
"            unsafe\n"
"                let fdata = data + x.offset\n"
"                var elem <- walk_and_convert ( fdata, x._type, at)\n"
"                var mkF <- new [[MakeFieldDecl() at=at, name := x.name, value <-"
" elem]]\n"
"                if !x._type.canCopy\n"
"                    mkF.flags = FieldDeclarationFlags moveSemantics\n"
"                emplace_new(*mkS, mkF)\n"
"    emplace(mkStruct.structs, mkS)\n"
"	return mkStruct\n"
"\n"
"def private walk_and_convert_variant ( data : uint8?; info:TypeDeclPtr; at:LineI"
"nfo ) : ExpressionPtr\n"
"    unsafe\n"
"        let vindex = * reinterpret<int?> data\n"
"        let offset = get_variant_field_offset(info,vindex)\n"
"        var mkVariant <- new [[ExprMakeVariant() at=at, makeType<-clone_type(inf"
"o)]]\n"
"        var elem <- walk_and_convert ( data + offset, info.argTypes[vindex], at)"
"\n"
"        var mkS <- new [[MakeFieldDecl() at=at, name:=info.argNames[vindex], val"
"ue <- elem]]\n"
"        if !info.argTypes[vindex].canCopy\n"
"            mkS.flags = FieldDeclarationFlags moveSemantics\n"
"        emplace(mkVariant.variants, mkS)\n"
"        return mkVariant\n"
"\n"
"def private walk_and_convert_table ( data : uint8?; info:TypeDeclPtr; at:LineInf"
"o ) : ExpressionPtr\n"
"    let total = any_table_size(data)\n"
"    if total != 0\n"
"        var tupT <- new [[TypeDecl() baseType=Type tTuple, at=at]]\n"
"        emplace_new(tupT.argTypes,clone_type(info.firstType))\n"
"        emplace_new(tupT.argTypes,clone_type(info.secondType))\n"
"        var arrT <- clone_type(tupT)\n"
"        var mkArr <- new [[ExprMakeArray() at=at, makeType <- arrT]]\n"
"        var key_stride = info.firstType.sizeOf\n"
"        var value_stride = info.secondType.sizeOf\n"
"        any_table_foreach ( data, key_stride, value_stride ) <| $ ( pkey, pvalue"
" )\n"
"            var key <- walk_and_convert ( pkey, info.firstType, at)\n"
"            var value <- walk_and_convert ( pvalue, info.secondType, at)\n"
"            var mkTup <- new [[ExprMakeTuple() at=at, recordType<-clone_type(tup"
"T), isKeyValue=true]]\n"
"            emplace(mkTup.values,key)\n"
"            emplace(mkTup.values,value)\n"
"            emplace(mkArr.values,mkTup)\n"
"        push(mkArr.makeType.dim,total)\n"
"        var mkToTableMove <- new [[ExprCall() at=at, name:=\"to_table_move\"]]\n"
"        emplace(mkToTableMove.arguments,mkArr)\n"
"        tupT := null\n"
"        return mkToTableMove\n"
"    else\n"
"        var mkArr <- new [[ExprMakeStruct() at=at, makeType<-clone_type(info)]]\n"
"        return mkArr\n"
"\n"
"def private walk_and_convert_basic ( data : uint8?; info:TypeDeclPtr; at:LineInf"
"o ) : ExpressionPtr\n"
"    unsafe\n"
"        if info.baseType == Type tInt\n"
"            return new [[ExprConstInt() at=at, value=*reinterpret<int?> data]]\n"
"        elif info.baseType == Type tUInt\n"
"            return new [[ExprConstUInt() at=at, value=*reinterpret<uint?> data]]"
"\n"
"        elif info.baseType == Type tFloat\n"
"            return new [[ExprConstFloat() at=at, value=*reinterpret<float?> data"
"]]\n"
"        elif info.baseType == Type tDouble\n"
"            return new [[ExprConstDouble() at=at, value=*reinterpret<double?> da"
"ta]]\n"
"        elif info.baseType == Type tBool\n"
"            return new [[ExprConstBool() at=at, value=*reinterpret<bool?> data]]"
"\n"
"        elif info.baseType == Type tRange\n"
"            return new [[ExprConstRange() at=at, value=*reinterpret<range?> data"
"]]\n"
"        elif info.baseType == Type tURange\n"
"            return new [[ExprConstURange() at=at, value=*reinterpret<urange?> da"
"ta]]\n"
"        elif info.baseType == Type tString\n"
"            return new [[ExprConstString() at=at, value:=*reinterpret<string?> d"
"ata]]\n"
"        else\n"
"            panic(\"unsupported {info.baseType}\")\n"
"            return [[ExpressionPtr]]\n"
"\n"
"def private walk_and_convert_enumeration ( data : uint8?; info:TypeDeclPtr; at:L"
"ineInfo ) : ExpressionPtr\n"
"    var eval = 0\n"
"    if info.baseType == Type tEnumeration\n"
"        unsafe\n"
"            eval = int(* reinterpret<uint?> data)\n"
"    elif info.baseType == Type tEnumeration8\n"
"        eval = int(* data)\n"
"    elif info.baseType == Type tEnumeration16\n"
"        unsafe\n"
"            eval = int(* reinterpret<uint16?> data)\n"
"    else\n"
"        panic(\"unsupported enumeration\")\n"
"    return new [[ExprConstEnumeration() enumType:=info.enumType, value:=info.enu"
"mType.list[eval].name]]\n"
"\n"
"def private walk_and_convert ( data : uint8?; info:TypeDeclPtr; at:LineInfo ) : "
"ExpressionPtr\n"
"	// print(\"0x{intptr(data)} {describe(info)}\\n\")\n"
"	if info.dim.length != 0\n"
"		return walk_and_convert_dim(data,info,at)\n"
"	elif info.baseType == Type tArray\n"
"		return walk_and_convert_array(data,info,at)\n"
"	elif info.baseType == Type tPointer\n"
"        unsafe\n"
"            if *reinterpret<uint8??>data == null\n"
"                return new [[ExprConstPtr() at=at]]\n"
"            elif info.firstType==null || info.firstType.isVoid\n"
"                return new [[ExprConstPtr() at=at]]\n"
"            else\n"
"                return walk_and_convert_pointer(data,info,at)\n"
"	elif info.baseType == Type tStructure\n"
"		return walk_and_convert_structure(data,info,at)\n"
"	elif info.baseType == Type tVariant\n"
"		return walk_and_convert_variant(data,info,at)\n"
"	elif info.baseType == Type tTuple\n"
"		return walk_and_convert_tuple(data,info,at)\n"
"	elif info.baseType == Type tTable\n"
"		return walk_and_convert_table(data,info,at)\n"
"    elif info.baseType == Type tEnumeration8 || info.baseType == Type tEnumerati"
"on16 || info.baseType == Type tEnumeration\n"
"        return walk_and_convert_enumeration(data,info,at)\n"
"    else\n"
"        return walk_and_convert_basic(data,info,at)\n"
"\n"
"def convert_to_expression ( var value : auto&; at:LineInfo )\n"
"	let info <- typeinfo(ast_typedecl value)\n"
"    unsafe\n"
"	    let pval = addr(value)\n"
"	    return walk_and_convert(reinterpret<uint8?> pval, info, at)\n"
"\n"
"def find_annotation ( mod_name,ann_name:string ) : Annotation const?\n"
"    var mod = find_compiling_module(mod_name)\n"
"    if mod==null\n"
"        return null\n"
"    var ann : Annotation const?\n"
"    module_for_each_annotation(mod) <| $ ( value )\n"
"        peek(value.name) <| $ ( vname )\n"
"            if vname == ann_name\n"
"                unsafe\n"
"                    ann = addr(value)\n"
"    return ann\n"
"\n"
"def append_annotation ( var func:FunctionPtr; mod_name,ann_name:string; args:arr"
"ay< tuple<argname:string;argvalue:RttiValue> > )\n"
"    var ann = find_annotation(mod_name, ann_name)\n"
"    assert(ann!=null)\n"
"    var decl <- new [[AnnotationDeclaration()]]\n"
"    unsafe\n"
"        decl.annotation := reinterpret<smart_ptr<Annotation>> ann\n"
"    for argP in args\n"
"        let argName & = argP.argname\n"
"        let arg & = argP.argvalue\n"
"        if arg is tBool\n"
"            decl.arguments |> add_annotation_argument(argName, arg as tBool)\n"
"        elif arg is tInt\n"
"            decl.arguments |> add_annotation_argument(argName, arg as tInt)\n"
"        elif arg is tFloat\n"
"            decl.arguments |> add_annotation_argument(argName, arg as tFloat)\n"
"        elif arg is tString\n"
"            decl.arguments |> add_annotation_argument(argName, arg as tString)\n"
"        else\n"
"            panic(\"Invalid rtti value\")\n"
"    add_function_annotation(func, decl)\n"
"    decl := null\n"
"\n"
"def add_annotation_argument ( var arguments:AnnotationArgumentList; argName:stri"
"ng; val:bool )\n"
"    let argIdx = arguments |> add_annotation_argument(argName)\n"
"    arguments[argIdx].basicType = Type tBool\n"
"    arguments[argIdx].bValue = val\n"
"    return argIdx\n"
"\n"
"def add_annotation_argument ( var arguments:AnnotationArgumentList; argName:stri"
"ng; val:int )\n"
"    let argIdx = arguments |> add_annotation_argument(argName)\n"
"    arguments[argIdx].basicType = Type tInt\n"
"    arguments[argIdx].iValue = val\n"
"    return argIdx\n"
"\n"
"def add_annotation_argument ( var arguments:AnnotationArgumentList; argName:stri"
"ng; val:float )\n"
"    let argIdx = arguments |> add_annotation_argument(argName)\n"
"    arguments[argIdx].basicType = Type tFloat\n"
"    arguments[argIdx].fValue = val\n"
"    return argIdx\n"
"\n"
"def add_annotation_argument ( var arguments:AnnotationArgumentList; argName:stri"
"ng; val:string )\n"
"    let argIdx = arguments |> add_annotation_argument(argName)\n"
"    arguments[argIdx].basicType = Type tString\n"
"    arguments[argIdx].sValue := val\n"
"    return argIdx\n"
"\n"
"def get_for_source_index(expr:smart_ptr<ExprFor>; svar:VariablePtr)\n"
"    for v,t in expr.iteratorVariables,range(100500)\n"
"        if v==svar\n"
"            return t\n"
"    return -1\n"
"\n"
"def get_for_source_index(expr:smart_ptr<ExprFor>; source:ExpressionPtr)\n"
"    for s,t in expr.sources,range(100500)\n"
"        if s==source\n"
"            return t\n"
"    return -1\n"
"\n"
"[_macro]\n"
"def private setup\n"
"    if is_compiling_macros_in_module(\"ast_boost\")\n"
"        add_new_function_annotation(\"macro\", new MacroMacro())\n"
"        add_new_function_annotation(\"tag_function\",new TagFunctionAnnotation())\n"
"        add_new_structure_annotation(\"function_macro\",new SetupFunctionAnnotatoi"
"n())\n"
"        add_new_structure_annotation(\"block_macro\",new SetupBlockAnnotatoin())\n"
"        add_new_structure_annotation(\"structure_macro\",new SetupStructureAnnotat"
"oin())\n"
"        add_new_structure_annotation(\"enumeration_macro\",new SetupEnumerationAnn"
"otation())\n"
"        add_new_structure_annotation(\"contract\",new SetupContractAnnotatoin())\n"
"        add_new_structure_annotation(\"reader_macro\",new SetupReaderMacro())\n"
"        add_new_structure_annotation(\"call_macro\",new SetupCallMacro())\n"
"        add_new_structure_annotation(\"typeinfo_macro\",new SetupTypeInfoMacro())\n"
"        add_new_structure_annotation(\"variant_macro\",new SetupVariantMacro())\n"
"        add_new_structure_annotation(\"tag_structure\",new TagStructureAnnotation("
"))\n"
"        add_new_structure_annotation(\"tag_function_macro\",new TagFunctionMacro()"
")\n"
"        add_new_structure_annotation(\"infer_macro\",new SetupInferMacro())\n"
"        add_new_structure_annotation(\"dirty_infer_macro\",new SetupDirtyInferMacr"
"o())\n"
"        add_new_structure_annotation(\"optimization_macro\",new SetupOptimizationM"
"acro())\n"
"        add_new_structure_annotation(\"lint_macro\",new SetupLintMacro())\n"
"        add_new_structure_annotation(\"global_lint_macro\",new SetupGlobalLintMacr"
"o())\n"
"        add_new_variant_macro(\"better_rtti_in_expr\", new BetterRttiVisitor())\n"
