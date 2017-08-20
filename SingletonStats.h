#ifndef __SINGLETON_STATS_H
#define __SINGLETON_STATS_H
class SingletonStats
{
	public:
	unsigned long n_try_connections;
	unsigned long n_failed_connections;
	unsigned long n_scanned;
	unsigned long n_messages_sent;
	unsigned long n_messages_received;
	unsigned long n_messages_repeatly;
	unsigned long n_messages_wasHere;
	unsigned long n_messages_removed;
	unsigned long n_sleeps;
    static SingletonStats *s_instance;
    SingletonStats()
    {
		 n_try_connections=0;
		 n_failed_connections=0;
		 n_scanned=0;
		 n_messages_sent=0;
		 n_messages_received=0;
		 n_messages_repeatly=0;
		 n_messages_wasHere=0;
		 n_messages_removed=0;
		 n_sleeps=0;
    }
  
    static SingletonStats *instance()
    {
        if (!s_instance)
          s_instance = new SingletonStats;
        return s_instance;
    }
};
#endif