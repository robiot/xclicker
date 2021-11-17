import { Link } from "react-router-dom";
import Layout from "../components/Layout";
import Nav from "../components/Nav";
import { ArrowDownward, GitHub } from "@material-ui/icons";

export default function Home() {
  return (
    <Layout title="Autoclicker For Linux" home={true}>
      <div className="bg-custom_gray">
        <Nav />

        <div className="flex justify-between text-white container-xl mx-auto px-10">
          <div className="mb-10 mt-16">
            <div className="text-3xl font-semibold mb-4">
              Autoclicker For Linux
            </div>
            <div className="w-full md:w-96">
              <p className="text-lg mb-4">
                XClicker is an open-source, easy to use, feature-rich and
                blazing fast Autoclicker for linux desktops using x11.
              </p>
              <p className="text-lg mb-8">
                It is written in C and uses the gtk framework. The
                user-interface may look different depending on what gtk theme
                you are using.
              </p>
            </div>

            <div className="flex flex-wrap">
              <Link to="/downloads" className="bg-blue-400 p-3 rounded-md text-white h-full text-lg mr-3 hidden sm:block">
                  <ArrowDownward width="0" />
                  <span className="ml-2">Download</span>
              </Link>

              <a
                href="https://github.com/robiot/xclicker"
                target="_blank"
                rel="noopener noreferrer"
                className="bg-gray-100 p-3 rounded-md text-gradient-gray h-full text-lg text-gray-600"
              >
                <GitHub className="mb-1" width="0" />
                <span className="ml-2">Source code</span>
              </a>
            </div>
          </div>
          <div className="mt-10 hidden lg:block">
            <img src="/window.png" className="h-98 mb-14" />
          </div>
        </div>
      </div>
      <div className="flex-wrap md:flex justify-between text-white container-xl mx-auto px-10 w-full mt-20 flex-grow">
        <div>
          <div className="text-3xl font-semibold mb-2 ">Youtube Video</div>
          <p className="text-lg">
            Here you can see a simple video of me testing xclicker.
          </p>
          <p className="text-lg mb-4">Sadly the audio dissapeared</p>
        </div>
        <iframe
          frameBorder="0"
          allowFullScreen
          src="https://www.youtube.com/embed/Lu0gTYSPzeQ"
          className=" w-full md:w-2/4 h-48 sm:h-72 md:h-80"
        ></iframe>
      </div>

      <div className="bg-gray-800 bg-opacity-40 mt-5 text-center p-2">
        copyright © robiot
      </div>
    </Layout>
  );
}
