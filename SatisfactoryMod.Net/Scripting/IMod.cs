using System.Threading.Tasks;

namespace SatisfactoryMod.Net.Scripting
{
    public interface IMod
    {
        Task OnStartAsync();
        Task OnStopAsync();
    }
}