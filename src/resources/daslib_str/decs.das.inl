//
// AUTO-GENERATED FILE - DO NOT EDIT!!
//

"options indenting = 4\n"
"options no_unused_block_arguments = false\n"
"options no_unused_function_arguments = false\n"
"\n"
"module decs shared public\n"
"\n"
"require daslib/templates\n"
"\n"
"require daslib/rtti public\n"
"require daslib/algorithm\n"
"require daslib/sort_boost\n"
"require daslib/functional\n"
"\n"
"typedef ComponentHash = uint\n"
"\n"
"struct CTypeInfo\n"
"    basicType : Type\n"
"    mangledName : string\n"
"    hash : uint\n"
"    size : uint\n"
"\n"
"struct public Component\n"
"    name : string\n"
"    hash : ComponentHash\n"
"    stride : int\n"
"    data : array<uint8>\n"
"    [[do_not_delete]] text : array<string>\n"
"    info : CTypeInfo\n"
"\n"
"struct public EntityId\n"
"    id : uint\n"
"    generation : int\n"
"\n"
"struct public Archetype\n"
"    hash : ComponentHash\n"
"    components : array<Component>\n"
"    size : int\n"
"    eidIndex : int\n"
"\n"
"struct public ComponentValue\n"
"    name : string\n"
"    info : CTypeInfo\n"
"    data : float4[4]\n"
"\n"
"typedef DeferEval = lambda<(var act:DeferAction):void>\n"
"\n"
"struct private DeferAction\n"
"    eid : EntityId\n"
"    action : DeferEval\n"
"\n"
"typedef ComponentMap = array<ComponentValue>\n"
"\n"
"struct public EcsRequest\n"
"    hash : ComponentHash\n"
"    req : array<string>\n"
"    reqn : array<string>\n"
"    archetypes : array<int>     // sorted\n"
"\n"
"struct public DecsState\n"
"    archetypeLookup : table<ComponentHash; int>\n"
"    allArchetypes : array<Archetype>\n"
"    entityFreeList : array<EntityId>\n"
"    entityLookup : array<tuple<generation:int;archetype:ComponentHash;index:int>"
">\n"
"    componentTypeCheck : table<string; CTypeInfo>\n"
"    ecsQueries : array<EcsRequest>\n"
"    queryLookup : table<uint; int>\n"
"\n"
"typedef PassFunction = function<():void>\n"
"\n"
"struct public DecsPass\n"
"    name : string\n"
"    calls : array<PassFunction>\n"
"\n"
"var public decsState : DecsState\n"
"var private deferActions : array<DeferAction>\n"
"var private decsPasses : array<DecsPass>\n"
"\n"
"def public register_decs_stage_call ( name:string; pcall:PassFunction )\n"
"    var dpass <- [[DecsPass name=name]]\n"
"    let idx = lower_bound(decsPasses,dpass) <| $ ( x,y ) => x.name < y.name\n"
"    if idx<length(decsPasses) && decsPasses[idx].name==name\n"
"        decsPasses[idx].calls |> push(pcall)\n"
"    else\n"
"        dpass.calls |> push(pcall)\n"
"        decsPasses |> emplace(dpass, idx)    // insert new one\n"
"\n"
"def public decs_stage ( name : string )\n"
"    commit()\n"
"    let idx = lower_bound(decsPasses,[[DecsPass name=name]]) <| $ ( x,y ) => x.n"
"ame < y.name\n"
"    if idx<length(decsPasses) && decsPasses[idx].name==name\n"
"        for cll in decsPasses[idx].calls\n"
"            invoke ( cll )\n"
"    commit()\n"
"\n"
"def public restart\n"
"    delete deferActions\n"
"    delete decsState\n"
"\n"
"def private new_entity_id()\n"
"    var eid : EntityId\n"
"    if !empty(decsState.entityFreeList)\n"
"        eid = decsState.entityFreeList[length(decsState.entityFreeList)-1]\n"
"        decsState.entityFreeList |> pop\n"
"    else\n"
"        eid.id = uint(length(decsState.entityLookup))\n"
"        decsState.entityLookup |> push([[auto 0,0u,0]])\n"
"    eid.generation ++\n"
"    return eid\n"
"\n"
"def public before_gc\n"
"    for arch in decsState.allArchetypes\n"
"        if arch.size > 0\n"
"            for comp in arch.components\n"
"                if comp.info.basicType==Type tString\n"
"                    unsafe\n"
"                        _builtin_make_temp_array(comp.text, addr(comp.data[0]), "
"arch.size )\n"
"                        __builtin_array_lock(comp.text)\n"
"\n"
"def public debug_dump\n"
"    for arch in decsState.allArchetypes\n"
"        print(\"archtype {arch.hash} : {arch.size}\\n\")\n"
"        // debug(arch)\n"
"        for index in range(arch.size)\n"
"            print(\"\\tentity[{index}]\\n\")\n"
"            for c in arch.components\n"
"                unsafe\n"
"                    print(\"\\t\\t{c.name} : {c.info}\\n\")\n"
"    for erq in decsState.ecsQueries\n"
"        print(\"query {erq.hash}\\n\")\n"
"        print(\"\\treq = {erq.req}\\n\\treqn = {erq.reqn}\\n\\tarchetypes = {erq.arche"
"types}\\n\")\n"
"\n"
"def private with_archetype ( hash:ComponentHash; blk : block<(var arch:Archetype"
"; idx:int; isNew:bool ):void> )\n"
"    var afound & = decsState.archetypeLookup[hash]\n"
"    if afound==0\n"
"        decsState.allArchetypes |> emplace([[Archetype hash=hash]])\n"
"        afound = length(decsState.allArchetypes)\n"
"        invoke(blk, decsState.allArchetypes[afound-1], afound-1, true)\n"
"    else\n"
"        invoke(blk, decsState.allArchetypes[afound-1], afound-1, false)\n"
"\n"
"def private create_archetype ( var arch:Archetype; cmp:ComponentMap; idx:int )\n"
"    assert(length(arch.components)==0)\n"
"    arch.eidIndex = -1\n"
"    for kv,kvi in cmp,range(INT_MAX)\n"
"        var ct & = decsState.componentTypeCheck[kv.name]\n"
"        if ct.hash != 0u\n"
"            if kv.info.hash != ct.hash\n"
"                panic(\"component {kv.name} type mismatch {ct} vs {kv.info}\")\n"
"        else\n"
"            ct = kv.info\n"
"        let chash = hash(kv.name)\n"
"        arch.components |> emplace <| [[Component\n"
"            name=kv.name,\n"
"            hash=chash,\n"
"            stride=int(kv.info.size),\n"
"            info=kv.info\n"
"        ]]\n"
"        if kv.name==\"eid\"\n"
"            assert(arch.eidIndex==-1)\n"
"            arch.eidIndex = kvi\n"
"    assert(arch.eidIndex!=-1)\n"
"    for erq in decsState.ecsQueries\n"
"        if erq |> can_process_request(arch)\n"
"            erq.archetypes |> push(idx)\n"
"\n"
"def private get_eid ( var arch:Archetype; index:int ) : EntityId &\n"
"    unsafe\n"
"        var ceid & = arch.components[arch.eidIndex]\n"
"        return *(reinterpret<EntityId?> addr(ceid.data[index*ceid.stride]))\n"
"\n"
"def private create_entity ( var arch:Archetype; eid:EntityId; cmp:ComponentMap )"
"\n"
"    let eidx = arch.size++\n"
"    for c,comp in arch.components,cmp\n"
"        c.data |> resize(length(c.data) + c.stride)\n"
"        unsafe\n"
"            memcpy ( addr(c.data[eidx*c.stride]), addr(comp.data), c.stride )\n"
"    return eidx\n"
"\n"
"def private remove_entity ( var arch:Archetype; di:int )\n"
"    arch.size --\n"
"    if di!=arch.size    // copy last one in the hole\n"
"        var eid_last & = arch |> get_eid(arch.size)\n"
"        decsState.entityLookup[eid_last.id].index = di\n"
"        for c in arch.components\n"
"            unsafe\n"
"                memcpy ( addr(c.data[di*c.stride]), addr(c.data[arch.size*c.stri"
"de]), c.stride )\n"
"    for c in arch.components\n"
"        c.data |> resize ( arch.size * c.stride )\n"
"\n"
"def private cmp_archetype_hash ( cmp:ComponentMap )\n"
"    var ahash : ComponentHash\n"
"    for kv in cmp\n"
"        ahash = (ahash<<<2u) ^ hash(kv.name)\n"
"    return ahash\n"
"\n"
"def private req_hash ( erq : EcsRequest )\n"
"    var ahash : ComponentHash\n"
"    for kv in erq.req\n"
"        ahash = (ahash<<<2u) ^ hash(kv)\n"
"    for kv in erq.reqn\n"
"        ahash = (ahash<<<2u) ^ ~hash(kv)\n"
"    return ahash\n"
"\n"
"def public has ( arch:Archetype; name:string )\n"
"    return arch.components |> binary_search ( [[Component name=name]] ) <| $ ( x"
", y ) => x.name < y.name\n"
"\n"
"def private can_process_request ( var erq : EcsRequest; var arch : Archetype )\n"
"    if erq.hash==arch.hash\n"
"        return true\n"
"    for r in erq.req\n"
"        if ! arch |> has(r)\n"
"            return false\n"
"    for r in erq.reqn\n"
"        if arch |> has(r)\n"
"            return false\n"
"    return true\n"
"\n"
"def public verify_request ( var erq : EcsRequest ) : tuple<ok:bool;error:string>"
"\n"
"    if erq.hash==0u || (empty(erq.req) && empty(erq.reqn))\n"
"        return [[auto false, \"missing components\"]]\n"
"    for N in erq.reqn   // assuming require_not is typically shorter\n"
"        if erq.req |> binary_search(N)\n"
"            return [[auto false, \"duplicate req and neq {N}\"]]\n"
"    return [[auto true,\"\"]]\n"
"\n"
"def public compile_request ( var erq : EcsRequest )\n"
"    sort_unique(erq.req)\n"
"    sort_unique(erq.reqn)\n"
"    erq.hash = req_hash(erq)\n"
"\n"
"def public lookup_request ( var erq : EcsRequest )\n"
"    if erq.hash==0u\n"
"        compile_request(erq)\n"
"    var ql & = decsState.queryLookup[erq.hash]\n"
"    if ql == 0\n"
"        for arch,archi in decsState.allArchetypes,range(INT_MAX)\n"
"            if erq |> can_process_request(arch)\n"
"                erq.archetypes |> push(archi)\n"
"        decsState.ecsQueries |> push_clone(erq)\n"
"        ql = length(decsState.ecsQueries)\n"
"    return ql - 1\n"
"\n"
"def public for_each_archetype ( var erq : EcsRequest; blk:block<(arch:Archetype)"
":void> )\n"
"    let qi = lookup_request(erq)\n"
"    for aidx in decsState.ecsQueries[qi].archetypes\n"
"        var arch & = decsState.allArchetypes[aidx]\n"
"        if arch.size > 0\n"
"            invoke ( blk, arch )\n"
"\n"
"def public for_eid_archetype ( eid:EntityId implicit; hash:ComponentHash; var er"
"q : function<():EcsRequest>; blk:block<(arch:Archetype;index:int):void> )\n"
"    var lookup & = decsState.entityLookup[eid.id]\n"
"    if lookup.generation != eid.generation\n"
"        return false\n"
"    var qi = -1\n"
"    decsState.queryLookup |> find_if_exists(hash) <| $ ( ql )\n"
"        qi = *ql - 1\n"
"    if qi == -1\n"
"        qi = lookup_request(invoke(erq))\n"
"    var req & = decsState.ecsQueries[qi]\n"
"    let aidx = decsState.archetypeLookup[lookup.archetype]\n"
"    if aidx == 0\n"
"        panic(\"archetype not found\")\n"
"    if binary_search(req.archetypes,aidx-1)\n"
"        var arch & = decsState.allArchetypes[aidx-1]\n"
"        assert(arch.size > 0)\n"
"        invoke(blk, arch, lookup.index)\n"
"        return true\n"
"    else\n"
"        panic(\"archetype not found\")\n"
"        return false\n"
"\n"
"def public for_each_archetype ( hash:ComponentHash; var erq : function<():EcsReq"
"uest>; blk:block<(arch:Archetype):void> )\n"
"    var qi = -1\n"
"    decsState.queryLookup |> find_if_exists(hash) <| $ ( ql )\n"
"        qi = *ql - 1\n"
"    if qi == -1\n"
"        qi = lookup_request(invoke(erq))\n"
"    for aidx in decsState.ecsQueries[qi].archetypes\n"
"        var arch & = decsState.allArchetypes[aidx]\n"
"        if arch.size > 0\n"
"            invoke ( blk, arch )\n"
"\n"
"def public for_each_archetype_find ( hash:ComponentHash; var erq : function<():E"
"csRequest>; blk:block<(arch:Archetype):bool> )\n"
"    var qi = -1\n"
"    decsState.queryLookup |> find_if_exists(hash) <| $ ( ql )\n"
"        qi = *ql - 1\n"
"    if qi == -1\n"
"        qi = lookup_request(invoke(erq))\n"
"    for aidx in decsState.ecsQueries[qi].archetypes\n"
"        var arch & = decsState.allArchetypes[aidx]\n"
"        if arch.size > 0\n"
"            if invoke ( blk, arch )\n"
"                return true\n"
"    return false\n"
"\n"
"[template(atype)]\n"
"def decs_array ( atype:auto(TT); src:array<uint8>; capacity:int ) : array<TT-con"
"st-&>#\n"
"    assert(length(src)>0)\n"
"    var dest : array<TT -const -&>#\n"
"    unsafe\n"
"        _builtin_make_temp_array(dest, addr(src[0]), capacity)\n"
"    __builtin_array_lock(dest)\n"
"    unsafe\n"
"        return <- dest\n"
"\n"
"def public get ( arch:Archetype; name:string; value:auto(TT) ) : array<TT-const-"
"&>#\n"
"    let idx = arch.components |> lower_bound([[Component name=name]]) <| $ ( x,y"
" ) => x.name < y.name\n"
"    if idx<length(arch.components)\n"
"        let comp & = arch.components[idx]\n"
"        if comp.name==name\n"
"            unsafe\n"
"                let cvinfo = addr(typeinfo(rtti_typeinfo type<TT-const-&>))\n"
"                if comp.info.hash != cvinfo.hash\n"
"                    panic(\"component array {name} type mismatch, expecting {comp"
".info} vs {describe(cvinfo)}\")\n"
"            unsafe\n"
"                return <- decs_array(type<TT>, comp.data, arch.size)\n"
"    panic(\"component array {name} not found\")\n"
"    unsafe\n"
"        return <- [[array<TT-const-&># ]]\n"
"\n"
"def public get_ro ( arch:Archetype; name:string; value:auto(TT) ) : array<TT-con"
"st-&> const#\n"
"    unsafe\n"
"        return <- get(arch, name, value)\n"
"\n"
"def public get_default_ro ( arch:Archetype; name:string; value:auto(TT) ) : iter"
"ator<TT const &>\n"
"    let idx = arch.components |> lower_bound([[Component name=name]]) <| $ ( x,y"
" ) => x.name < y.name\n"
"    if idx<length(arch.components)\n"
"        let comp & = arch.components[idx]\n"
"        if comp.name==name\n"
"            unsafe\n"
"                let cvinfo = addr(typeinfo(rtti_typeinfo type<TT-const-&>))\n"
"                if comp.info.hash != cvinfo.hash\n"
"                    panic(\"component array {name} type mismatch, expecting {comp"
".info} vs {describe(cvinfo)}\")\n"
"            var it : iterator<TT const &>\n"
"            unsafe\n"
"                _builtin_make_fixed_array_iterator(it,addr(comp.data[0]),arch.si"
"ze,comp.stride)\n"
"            return <- it\n"
"    return <- repeat_ref(value,arch.size)\n"
"\n"
"def private update_entity_imm ( eid:EntityId; blk : lambda<(eid:EntityId; var cm"
"p:ComponentMap):void> )\n"
"    var lookup & = decsState.entityLookup[eid.id]\n"
"    if lookup.generation != eid.generation\n"
"        return\n"
"    var cmp : ComponentMap\n"
"    var arch & = decsState.allArchetypes[decsState.archetypeLookup[lookup.archet"
"ype]-1]\n"
"    let eidx = decsState.entityLookup[eid.id].index\n"
"    cmp |> reserve ( length(arch.components) )\n"
"    for c in arch.components\n"
"        var value = [[ComponentValue name=c.name, info=c.info]]\n"
"        unsafe\n"
"            memcpy ( addr(value.data), addr(c.data[eidx*c.stride]), c.stride)\n"
"        cmp |> push(value)\n"
"    var old_ahash = arch.hash\n"
"    invoke(blk, eid, cmp)\n"
"    cmp |> set(\"eid\", eid)  // necessary?\n"
"    var new_ahash = cmp_archetype_hash(cmp)\n"
"    if old_ahash == new_ahash\n"
"        for c,comp in arch.components,cmp\n"
"            unsafe\n"
"                memcpy ( addr(c.data[eidx*c.stride]), addr(comp.data), c.stride "
")\n"
"    else\n"
"        arch |> remove_entity ( eidx )\n"
"        with_archetype(new_ahash) <| $ ( var narch; idx; isNew )\n"
"            if isNew\n"
"                narch |> create_archetype(cmp, idx)\n"
"            let neidx = narch |> create_entity(eid, cmp)\n"
"            decsState.entityLookup[eid.id] = [[auto eid.generation,new_ahash,nei"
"dx]]\n"
"    delete cmp\n"
"\n"
"def private create_entity_imm ( eid:EntityId; blk : lambda<(eid:EntityId; var cm"
"p:ComponentMap):void> )\n"
"    var cmp : ComponentMap\n"
"    cmp |> push <| make_component(\"eid\", eid)\n"
"    invoke(blk, eid, cmp)\n"
"    cmp |> set(\"eid\", eid)       // necessary?\n"
"    var ahash = cmp_archetype_hash(cmp)\n"
"    with_archetype(ahash) <| $ ( var arch; idx; isNew )\n"
"        if isNew\n"
"            arch |> create_archetype(cmp, idx)\n"
"        let eidx = arch |> create_entity(eid,cmp)\n"
"        decsState.entityLookup[eid.id] = [[auto eid.generation,ahash,eidx]]\n"
"    delete cmp\n"
"\n"
"def private delete_entity_imm ( eid:EntityId )\n"
"    var lookup & = decsState.entityLookup[eid.id]\n"
"    if lookup.generation == eid.generation\n"
"        var arch & = decsState.allArchetypes[decsState.archetypeLookup[lookup.ar"
"chetype]-1]\n"
"        let di = decsState.entityLookup[eid.id].index\n"
"        decsState.entityLookup[eid.id].generation = 0\n"
"        decsState.entityFreeList |> push(eid)\n"
"        arch |> remove_entity(di)\n"
"\n"
"def public update_entity ( entityid:EntityId implicit; var blk : lambda<(eid:Ent"
"ityId; var cmp:ComponentMap):void> )\n"
"    var deval <- @ <| [[<-blk]] ( var act:DeferAction )\n"
"        update_entity_imm(act.eid,blk)\n"
"    deferActions |> emplace([[DeferAction action<-deval, eid=entityid]])\n"
"\n"
"def public create_entity ( var blk : lambda<(eid:EntityId; var cmp:ComponentMap)"
":void> )\n"
"    var deval <- @ <| [[<-blk]] ( var act:DeferAction )\n"
"        create_entity_imm(act.eid, blk)\n"
"    var eid = new_entity_id()\n"
"    deferActions |> emplace([[DeferAction action<-deval, eid=eid]])\n"
"    return eid\n"
"\n"
"def public delete_entity ( entityid:EntityId implicit )\n"
"    var deval <- @ <| ( var act:DeferAction )\n"
"        delete_entity_imm(act.eid)\n"
"    deferActions |> emplace([[DeferAction action<-deval, eid=entityid]])\n"
"\n"
"def public commit\n"
"    for da in deferActions\n"
"        da.action |> invoke(da)\n"
"    delete deferActions\n"
"\n"
"def public get ( var cmp:ComponentMap; name:string; var value:auto(TT) )\n"
"    let idx = lower_bound(cmp,[[ComponentValue name=name]]) <| $ ( x,y ) => x.na"
"me < y.name\n"
"    if idx<length(cmp) && cmp[idx].name==name\n"
"        unsafe\n"
"            let cvinfo = addr(typeinfo(rtti_typeinfo type<TT-const-&-#>))\n"
"            if cmp[idx].info.hash != cvinfo.hash\n"
"                panic(\"get component {name} type mismatch, expecting {cmp[idx].i"
"nfo} vs {describe(cvinfo)}\")\n"
"            memcpy ( addr(value), addr(cmp[idx].data), typeinfo(sizeof type<TT c"
"onst-&>))\n"
"    return value\n"
"\n"
"def private make_component ( name:string; value:auto(TT) )\n"
"    var cv = [[ComponentValue name=name]]\n"
"    unsafe\n"
"        var tinfo = addr(typeinfo(rtti_typeinfo type<TT-const-&-#>))\n"
"        cv.info = [[CTypeInfo basicType=tinfo.basicType, mangledName=get_mangled"
"_name(tinfo), hash=tinfo.hash, size=tinfo.size]]\n"
"        memcpy ( addr(cv.data), addr(value), typeinfo(sizeof value))\n"
"    return cv\n"
"\n"
"def public set ( var cmp:ComponentMap; name:string; value:auto(TT) )\n"
"    concept_assert((typeinfo(is_pod value) || typeinfo(is_string value))\n"
"        && typeinfo(sizeof value)<=typeinfo(sizeof cmp[0].data), \"unsupported co"
"mponent type {typeinfo(typename value)}\")\n"
"    let cv = make_component(name, value)\n"
"    let idx = lower_bound(cmp,[[ComponentValue name=name]]) <| $ ( x,y ) => x.na"
"me < y.name\n"
"    if idx<length(cmp) && cmp[idx].name==name\n"
"        if cmp[idx].info.hash != cv.info.hash\n"
"            panic(\"set component {name} type mismatch, expecting {cmp[idx].info}"
" vs {cv.info}\")\n"
"        cmp[idx] = cv           // overwrite\n"
"    else\n"
"        cmp |> push(cv, idx)    // insert new one\n"
"\n"
"def public has ( var cmp:ComponentMap; name:string )\n"
"    return binary_search(cmp,[[ComponentValue name=name]]) <| $ ( x,y ) => x.nam"
"e < y.name\n"
"\n"
"def public remove ( var cmp:ComponentMap; name:string )\n"
"    let idx = lower_bound(cmp,[[ComponentValue name=name]]) <| $ ( x,y ) => x.na"
"me < y.name\n"
"    if idx<length(cmp) && cmp[idx].name==name\n"
"        cmp |> erase(idx)\n"
