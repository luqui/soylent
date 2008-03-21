module Soy
    module SDL
        class Keysym
            attr_reader :scancode, :sym, :mod, :unicode
        end

        class Event
        end
        
        class ActiveEvent < Event
            attr_reader :gain, :state
        end

        class KeyboardEvent < Event
            attr_reader :state, :keysym
        end

        class KeyUpEvent < KeyboardEvent
        end

        class KeyDownEvent < KeyboardEvent
        end

        class MouseMotionEvent < Event
            attr_reader :state, :x, :y, :xrel, :yrel
        end

        class MouseButtonEvent < Event
            attr_reader :button, :state, :x, :y
        end

        class MouseButtonUpEvent < MouseButtonEvent
        end

        class MouseButtonDownEvent < MouseButtonEvent
        end

        class JoyAxisEvent < Event
            attr_reader :which, :axis, :value
        end

        class JoyBallEvent < Event
            attr_reader :which, :ball, :xrel, :yrel
        end

        class JoyHatEvent < Event
            attr_reader :which, :hat, :value
        end

        class JoyButtonEvent < Event
            attr_reader :which, :button, :state
        end

        class JoyButtonUpEvent < JoyButtonEvent
        end

        class JoyButtonDownEvent < JoyButtonEvent
        end
    
        class ResizeEvent < Event
            attr_reader :w, :h
        end

        class ExposeEvent < Event
        end

        class QuitEvent < Event
        end

    end

    module ManyMouse
        class Event
            attr_reader :device, :item, :value, :minval, :maxval
        end

        class AbsMotionEvent < Event
        end

        class RelMotionEvent < Event
        end

        class ButtonEvent < Event
        end

        class ScrollEvent < Event
        end

        class DisconnectEvent < Event
        end
    end

    # we have two drivers for mouse input, we'd like a consistent
    # interface
    module UniformMouse
        class Event
        end

        class MouseMotionEvent
            attr_reader :device, :x, :y, :xrel, :yrel
            def init(device, x, y, xrel, yrel)
                super()
                @x, @y, @xrel, @yrel = x, y, xrel, yrel
            end
        end

        class MouseButtonEvent
            attr_reader :device, :button
            def init(device, button)
                super()
                @button = button
            end
        end

        class MouseButtonUpEvent < MouseButtonEvent
        end

        class MouseButtonDownEvent < MouseButtonEvent
        end

        
        class MouseReader
            def init
                super
                @statedata = {}
            end

            def convert(e)
                if ManyMouse::Event === e
                    # XXX this = 0 thing is a hack, but I don't think there's any other
                    # way to get the information.
                    @statedata[e.device] ||= { :x => 0, :y => 0, :name => ManyMouse.device_name(e.device) }
                end
                
                case e
                when SDL::MouseMotionEvent
                    return MouseMotionEvent.new("SDL", e.x, e.y, e.xrel, e.yrel)
                when SDL::MouseButtonUpEvent
                    return MouseButtonUpEvent.new("SDL", e.button)
                when SDL::MouseButtonDownEvent
                    return MouseButtonDownEvent.new("SDL", e.button)
                when ManyMouse::AbsMotionEvent
                    state = @state[e.device]
                    if e.item == 0
                        xrel, yrel = e.value - state[:x], 0
                        state[:x] = e.value
                    elsif e.item == 1
                        xrel, yrel = 0, e.value - state[:y]
                        state[:y] = e.value
                    end
                    return MouseMotionEvent.new(state[:name], state[:x], state[:y], xrel, yrel)
                when ManyMouse::RelMotionEvent
                    state = @state[e.device]
                    if e.item == 0
                        xrel, yrel = e.value, 0
                        state[:x] += xrel
                    elsif e.item == 1
                        xrel, yrel = 0, e.value
                        state[:y] += yrel
                    end
                    return MouseMotionEvent.new(state[:name], state[:x], state[:y], xrel, yrel)
                when ManyMouse::MouseButtonUpEvent
                    return MouseButtonUpEvent.new(@state[e.device][:name], e.item)
                when ManyMouse::MouseButtonDownEvent
                    return MouseButtonDownEvent.new(@state[e.device][:name], e.item)
                else
                    return e
                end
            end
        end
    end
end
