# -*- coding: utf-8 -*-
import json
def handler(event, context):
    input = json.loads(event)
    name = ''
    dev = {"text": "今天不用带伞"}
    
    if input.has_key('location'):
        name = name + input['location']
        # dev['device'] = dev['device'] + input['location']
    if input.has_key('size'):
        name = name + input['size']
        # dev['device'] = dev['device'] + input['size']
    if input.has_key('description'):
        name = name + input['description']
        # dev['device'] = dev['device'] + input['description']
    name = name + input['device']
    # dev['device'] = dev['device'] + input['device']
    dev['devname'] = name
    return json.dumps(dev)
    
    return json.dumps({"duiWidget": "text", "text": "今天不用带伞", "extra": { "key1": "val1", "key2": "val2"}})

handler('{"sdf":123,"location":"34","device":"helo"}', "123")
