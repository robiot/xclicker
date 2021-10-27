import Link from "next/link";
import Layout from "../components/Layout";
import Nav from "../components/Nav";
import { ArrowDownward, GitHub } from "@material-ui/icons";

export default function Home({ source }) {
  return (
    <Layout title="Autoclicker For Linux" home={true}>
      <div className="bg-gradient-to-br from-gradient-gray to-gradient-gray-less h-screen">
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
              <Link href="/downloads">
                <a className="bg-blue-400 p-3 rounded-md text-white h-full text-lg mr-3 hidden sm:block">
                  <ArrowDownward />
                  <span className="ml-2">Download</span>
                </a>
              </Link>


              <a
                href="https://github.com/robiot/XClicker"
                target="_blank"
                className="bg-gray-100 p-3 rounded-md text-gradient-gray h-full text-lg"
              >
                <GitHub className="mb-1" />
                <span className="ml-2">Source code</span>
              </a>
            </div>
          </div>
          <div className="mt-10 hidden lg:block">
            <img src="/window12.png" className="h-98" />
          </div>
        </div>
      </div>
    </Layout>
  );
}
