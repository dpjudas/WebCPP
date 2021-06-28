
#include "PodcastEpisode.h"
#include <WebCPP/Controls/TextLabel/TextLabel.h>
#include <WebCPP/Controls/ImageBox/ImageBox.h>

PodcastEpisode::PodcastEpisode(View* parent) : VBoxView(parent)
{
	title = new TextLabel(this);
	image = new ImageBox(this);
	description = new TextLabel(this);

	addClass("postcastepisode");
	title->addClass("postcastepisode-title");
	image->addClass("postcastepisode-image");
	description->addClass("postcastepisode-description");

	title->setText("Chicane Sun:Sets Vol 11");
	image->setSrc("https://www.thisisdistorted.com/repository/images/chicanepack.jpg");
	description->setHtml(
		"After the huge success of Chicane's international Sun:Sets show we are very pleased to announce the program and podcast will now be "
		"released every single week! rnrnGet involved by suggesting your favourite movie soundtrack, keep up to date with the dance music news, "
		"and hear some of the most eclectic electronic music around.rnrnPress the subscribe button now and get Sun:Sets delivered to your phone, "
		"tablet or computer for free every 7 days.<br /><br /> 1. D*Note - Garden of Earthly Delights <br />2. Joachim Pastor - Kenia<br />3. Danalog "
		"- Click, Search (Jody Wistenoff mix)<br />4. Choir of Young Believers - Hollow talk (Jody Wistenoff, James Grant & Lane 8 remix)<br />5. "
		"Bruce Springsteen - Secret Garden<br />6. The Sabres of Paradise - Smokebelch II (Beatless Mix)<br />7. Metrik - Dream Sequence<br />8. "
		"Metrik - Freefall<br />9. Grum - Straight to your heart<br />10. Chicane - Motion<br />11. Pryda - Melo<br />12. Michael Woods - Airbourne<br />");
}
