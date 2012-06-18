using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Collections.ObjectModel;
using System.ComponentModel.Composition;

namespace HiFreTradeUI.ServerConfig
{
    [Export]
    public class ServerConfiguation
    {
        public ServerConfiguation()
        {
            Load("broker.xml");
        }

        private ObservableCollection<ServerGroup> _serverGroups = new ObservableCollection<ServerGroup>();

        public IEnumerable<ServerGroup> Groups
        {
            get { return _serverGroups; }
        }

        public void Load(string configFile)
        {
            XElement xmlDoc = XElement.Load(configFile);
            var servGroups = xmlDoc.Elements("serverGroup");
            foreach (var sg in servGroups)
            {
                ServerGroup group = new ServerGroup();
                group.GroupName = sg.Attribute("name").Value;
                var serv_elems = sg.Elements("server");

                foreach (var sv in serv_elems)
                {
                    ServerInfo si = new ServerInfo();
                    si.Name = sv.Attribute("name").Value;
                    si.Address = sv.Attribute("address").Value;
                    XAttribute attrReal = sv.Attribute("isReal");
                    si.IsReal = attrReal != null ? bool.Parse(attrReal.Value) : true;
                    group.Servers.Add(si);
                }

                _serverGroups.Add(group);
            }
        }

    }
}
