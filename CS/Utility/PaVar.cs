using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Web.Script.Serialization;

using ObjList = System.Collections.Generic.List<object>;
using ObjMap = System.Collections.Generic.Dictionary<string,object>;

namespace XXCS
{
    class Logger
    {
        public static void Message(string msg)
        {
            Console.WriteLine(msg);
        }
    }

    class Pair
    {
        public Pair(object first, object second)
        {
            this.first = first;
            this.second = second;
        }
        public object first = null;
        public object second = null;
    }

    class PaKey
    {
        public PaKey() { _keys = new List<object>(); }
        public PaKey this[object key] {
            get
            {
                _keys.Add(key);
                return this;
            }
        }
        protected List<object> _keys;
    }

    class PaVar
    {
        public PaVar(){}
        public void Set(List<object> keys, object value)
        {
            if(keys.Count() == 0)
            {
                _root = value;
                return;
            }

            List<Pair> temp = new List<Pair>();
            AddNode(_root,ref temp,keys[0]);
            for (int index=1; index<keys.Count(); index++)
            {
                AddNode(ref temp,keys[index]);
            }

            Pair last = temp.Last();
            last.second = value;
            object key = last.first;
            object val = last.second;
            temp.Reverse();

            for (int index = 1; index<temp.Count(); index++)
            {
                object parent = temp[index].second;
                if (!SaveNode(ref parent, key, val))
                {
                    Logger.Message("[PaVar] [Set] warning.");
                    return;
                }
                temp[index].second = parent;

                key = temp[index].first;
                val = temp[index].second;
            }

            if (!SaveNode(ref _root,key,val))
            {
                Logger.Message("[PaVar] [Set] warning.");
            }
        }
        public object Get(List<object> keys)
        {
            object temp = _root;
            foreach (var key in keys)
            {
                if (key is int && temp is ObjList)
                {
                    int index = Convert.ToInt32(key);
                    var list = temp as ObjList;
                    if(index>=0 && index < list.Count())
                    {
                        temp = list[index];
                    }
                    else
                    {
                        return null;
                    }
                }
                else if (key is string && temp is ObjMap)
                {
                    string str = key as string;
                    var map = temp as ObjMap;
                    if (map.ContainsKey(str))
                    {
                        temp = map[str];
                    }
                    else
                    {
                        return null;
                    }
                }
                else
                {
                    return null;
                }
            }
            return temp;
        }
        public void ResetRoot(object root)
        {
            _root = root;
        }

        private void AddNode(ref List<Pair> varList, object key)
        {
            Pair last = varList.Last();
            if (key is int && null!=last.second && last.second is ObjList)
            {
                int index = Convert.ToInt32(key);
                ObjList list = (ObjList)last.second;
                varList.Add(new Pair(key, index>=list.Count() ? null : list[index]));
            }
            else if (key is string && null != last.second && last.second is ObjMap)
            {
                string str = Convert.ToString(key);
                ObjMap map = (ObjMap)last.second;
                varList.Add(new Pair(key, map.ContainsKey(str) ? map[str] : null));
            }
            else
            {
                varList.Add(new Pair(key, null));
            }
        }
        private void AddNode(object parent, ref List<Pair> varList, object key)
        {
            if (key is int && null!=parent && parent is ObjList)
            {
                int index = Convert.ToInt32(key);
                ObjList list = (ObjList)parent;
                varList.Add(new Pair(key, index >= list.Count() ? null : list[index]));
            }
            else if (key is string && null != parent && parent is ObjMap)
            {
                string str = Convert.ToString(key);
                ObjMap map = (ObjMap)parent;
                varList.Add(new Pair(key, map.ContainsKey(str) ? map[str] : null));
            }
            else
            {
                varList.Add(new Pair(key, null));
            }
        }
        private bool SaveNode(ref object parent, object key, object value)
        {
            if (key is int)
            {
                return SaveListNode(ref parent, Convert.ToInt32(key), value);
            }
            else if (key is string)
            {
                return SaveMapNode(ref parent, Convert.ToString(key), value);
            }
            else
            {
                return false;
            }
        }
        private bool SaveListNode(ref object parent, int index, object value)
        {
            if(parent is ObjList)
            {
                ObjList list = parent as ObjList;
                if(index>list.Count() || index < 0)
                {
                    Logger.Message("[PaVar] [SaveListNode] warning.");
                    return false;
                }
                else if (index==list.Count())
                {
                    list.Add(value);
                }
                else
                {
                    list[index] = value;
                }
                return true;
            }
            else if (null == parent)
            {
                if (0 != index)
                {
                    Logger.Message("[PaVar] [SaveListNode] warning.");
                    return false;
                }
                ObjList list = new ObjList();
                list.Add(value);
                parent = list;
                return true;
            }
            else
            {
                return false;
            }
        }
        private bool SaveMapNode(ref object parent, string str, object value)
        {
            if (null!=parent && !(parent is ObjMap))
            {
                Logger.Message("[PaVar] [SaveMapNode] warning.");
                return false;
            }

            ObjMap map = null;
            if(null == parent)
            {
                map = new ObjMap();
                parent = map;
            }
            else
            {
                map = parent as ObjMap;
            }
            map[str] = value;
            return true;
        }

        private string KeyString(List<object> keys) {
            return "";
        }
        private object _root = null;
    }

    class PaVarRef : PaKey
    {
        public PaVarRef(PaVar paVar)
        {
            _paVar = paVar;
        }
        public new PaVarRef this[object key]
        {
            get
            {
                PaVarRef varRef = new PaVarRef(_paVar);
                varRef._keys = new ObjList(this._keys);
                varRef._keys.Add(key);
                return varRef;
            }
        }
        public object GetValue()
        {
            return _paVar.Get(_keys);
        }
        public void SetValue(object value)
        {
            _paVar.Set(_keys, value);
        }

        public new string ToString()
        {
            return GetValue().ToString();
        }
        public int ToInt()
        {
            return Convert.ToInt32(GetValue());
        }
        public string ToJsonString()
        {
            JavaScriptSerializer jss = new JavaScriptSerializer();
            return jss.Serialize(GetValue());
        }
        public void FromJsonString(string json)
        {
            JavaScriptSerializer jss = new JavaScriptSerializer();
            object obj = jss.DeserializeObject(json);
            SetValue(obj);
        }

        private PaVarRef AddKey(object key)
        {
            _keys.Add(key);
            return this;
        }
        private PaVar _paVar;
    }
}
