require 'soy/events'

module Soy
    module Input
        class InputPattern
            attr_reader :ev_type, :params

            def init(ev_type, params)
                @ev_type, @params = ev_type, params
            end

            def match(e)
                return false unless @ev_type === e
                @params.each { |k,v|
                    return false unless e.send(k) == v
                }
                return true
            end

            def subtype_of?(pat)
                return false unless @ev_type <= pat.ev_type
                @params.each { |k,v|
                    return false if pat.params.has_key?(k) && pat.params[k] != v
                }
                return true
            end
        end

        class Mapping
            attr_reader :method, :bindings
        end

        class Allocator
            attr_reader :allocated

            def allocate(maplist)
                alloc = maplist.detect { |i| i.bindings.all { |k,v| !taken?(k) } }
                if alloc
                    @allocated << alloc
                    return alloc
                else
                    raise "Could not allocate"
                end
            end

            def taken?(pat)
                @allocated.each { |meth|
                    meth.bindings.each { |k,v|
                        return true if pat.subtype_of?(k)
                    }
                }
                return false
            end
        end
    end
end

mouse  = allocate Mouse
arrows = allocate Arrows

